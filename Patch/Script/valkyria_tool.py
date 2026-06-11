#!/usr/bin/env python3
import os
import sys
import csv
import struct
import argparse
import glob

# Character mapping for Italian characters in Valkyria Chronicles
ACCENT_MAP = {
    'é': '<',
    'è': '>',
    'à': '=',
    'ì': 'i',
    'ò': "o'",
    'ù': "u'",
    'Ì': "I'",
    'È': "E'"
}

def apply_character_replacements(text, is_mtp=False):
    """
    Applies Italian font map replacements and MTP-specific placeholders.
    """
    # 1. Apply standard Italian accents mapping
    for char, replacement in ACCENT_MAP.items():
        text = text.replace(char, replacement)
        
    if is_mtp:
        # MTP Specific replacements:
        # - Replace <Y> placeholder with bytes 0x81 0xA2.
        #   We use a special marker in Python that we will convert to raw bytes later.
        text = text.replace("<Y>", "\uE000") # Temporary unicode private area char for \x81\xA2
        
        # - Replace R£D with R&D
        text = text.replace("R£D", "R&D")
        
        # - Replace £ with space
        text = text.replace("£", " ")
        
        # - Replace & with newline \x0A
        text = text.replace("&", "\x0A")
        
        # - Replace ¤ with double quotes "
        text = text.replace("¤", '"')
    else:
        # MXE Specific replacements:
        # - Replace £ with double quotes
        text = text.replace("£", '"')
        
    return text

def text_to_bytes(text, is_mtp=False):
    """
    Converts string to bytes with UTF-8 encoding and handles special placeholder markers.
    """
    if is_mtp:
        # Encode character by character to handle the temporary \uE000 marker for \x81\xA2
        byte_list = []
        for char in text:
            if char == "\uE000":
                byte_list.extend([0x81, 0xA2])
            else:
                byte_list.extend(char.encode('utf-8', errors='replace'))
        return bytes(byte_list)
    else:
        return text.encode('utf-8', errors='replace')

def bytes_to_text(b, is_mtp=False):
    """
    Converts bytes to string and applies reverse replacements for MTP.
    """
    if is_mtp:
        # Decode special MTP characters back to CSV placeholders
        # 1. \x81\xA2 -> <Y>
        # To do this safely on bytes, we replace the byte sequence directly before decoding
        b_replaced = bytearray()
        idx = 0
        while idx < len(b):
            if idx + 1 < len(b) and b[idx] == 0x81 and b[idx+1] == 0xA2:
                b_replaced.extend(b"<Y>")
                idx += 2
            else:
                b_replaced.append(b[idx])
                idx += 1
        
        try:
            text = bytes(b_replaced).decode('utf-8')
        except UnicodeDecodeError:
            text = bytes(b_replaced).decode('cp1252', errors='replace')
        
        # 2. R&D -> R£D
        text = text.replace("R&D", "R£D")
        
        # 3. Newline \x0A -> &
        text = text.replace("\n", "&")
        
        # 4. Double quotes " -> ¤
        text = text.replace('"', "¤")
        
    else:
        try:
            text = b.decode('utf-8')
        except UnicodeDecodeError:
            text = b.decode('cp1252', errors='replace')
        
    return text

def is_printable_string(data, start_offset):
    """
    Verifies if a sequence of bytes at start_offset is a printable null-terminated string.
    """
    chars = []
    idx = start_offset
    while idx < len(data):
        b = data[idx]
        if b == 0:
            break
        # Printable ASCII + CP1252 accented characters + standard controls
        if (32 <= b <= 126) or (128 <= b <= 255) or b in (9, 10, 13):
            chars.append(b)
            idx += 1
        else:
            return None
    if not chars:
        return ""
    return bytes(chars)

# ==============================================================================
# MXE COMPILATION AND EXTRACTION
# ==============================================================================

def compile_mxe(csv_path, template_path, output_path, option_name):
    """
    Compiles an MXE CSV file back into binary format using a template.
    """
    print(f"Compiling MXE: {csv_path} using template {template_path}...")
    
    # 1. Read CSV rows
    rows = []
    with open(csv_path, "r", encoding="utf-8", errors="ignore") as f:
        reader = csv.reader(f, delimiter=';')
        for r in reader:
            if r:
                rows.append(r)
                
    if not rows:
        raise ValueError("CSV file is empty or invalid.")
        
    # Parse rows into structured list
    list_entries = []
    for idx, row in enumerate(rows):
        if len(row) < 4:
            continue
        addr_addr = row[0].strip()
        addr = row[1].strip()
        chapitre = row[2].strip()
        text_eng = row[3]
        
        # Dynamic column resolution: last non-empty column starting from index 4
        text_trad = ""
        col_idx = len(row) - 1
        if col_idx >= 0 and not row[col_idx] and len(row) > 2:
            col_idx = len(row) - 2
        if col_idx >= 4:
            text_trad = row[col_idx]
            
        # Apply replacements
        text_trad = apply_character_replacements(text_trad, is_mtp=False)
        
        list_entries.append({
            'Adresse_de_l_adresse': int(addr_addr),
            'Adresse': int(addr),
            'Chapitre': chapitre,
            'Texte_anglais': text_eng,
            'Texte_traduit': text_trad,
            'difftaille': len(text_trad) - len(text_eng)
        })
        
    count = len(list_entries)
    print(f"Parsed {count} translation rows.")
    
    # 2. Adjust addresses based on text length differences
    for i in range(count):
        diff = list_entries[i]['difftaille']
        if i + 1 < count and diff != 0:
            curr_addr = list_entries[i]['Adresse']
            next_raw_addr = list_entries[i+1]['Adresse']
            # If the next row starts at a different address, shift all subsequent addresses
            if next_raw_addr != curr_addr:
                for j in range(i + 1, count):
                    if list_entries[j]['Adresse'] > curr_addr:
                        list_entries[j]['Adresse'] += diff
                        
    # 3. Read template file
    with open(template_path, "rb") as f:
        file_bytes = bytearray(f.read())
        
    # 4. Patch pointer addresses (Little Endian 32-bit)
    for entry in list_entries:
        addr_ptr = entry['Adresse_de_l_adresse']
        addr_text = entry['Adresse']
        struct.pack_into("<I", file_bytes, addr_ptr, addr_text)
        
    # 5. Determine compilation mode: check if the template has a proper POF0 section
    # by reading the header offset at 0x34 which points to the data block whose size+0x40 = POF0
    try:
        orig_val_34_test = struct.unpack_from("<I", file_bytes, 0x34)[0]
        pof0_test_addr = orig_val_34_test + 0x40
        has_pof0 = (pof0_test_addr + 4 <= len(file_bytes) and 
                    file_bytes[pof0_test_addr:pof0_test_addr+4] == b"POF0")
    except Exception:
        has_pof0 = False
    
    if has_pof0:
        print(f"Applying POF0 reconstruction logic for {option_name}...")
        base_text_addr = list_entries[0]['Adresse'] + 32
        
        # Read header sizes
        orig_val_04 = struct.unpack_from("<I", file_bytes, 0x04)[0]
        orig_val_24 = struct.unpack_from("<I", file_bytes, 0x24)[0]
        orig_val_34 = struct.unpack_from("<I", file_bytes, 0x34)[0]
        
        pofo_orig_addr = orig_val_34 + 0x40
        print(f"Header POF0 original address: 0x{pofo_orig_addr:X}")
        
        if (pofo_orig_addr + 4 > len(file_bytes) or 
            file_bytes[pofo_orig_addr:pofo_orig_addr+4] != b"POF0"):
            raise ValueError(f"POF0 marker not found at expected header offset 0x{pofo_orig_addr:X}!")
            
        # Save POF0 and trailing block
        pofo_and_rest_block = file_bytes[pofo_orig_addr:]
        
        # Erase old text and everything after it
        del file_bytes[base_text_addr:]
        
        # Construct the new text block
        new_text_block = bytearray()
        base_text_data_address_new = list_entries[0]['Adresse'] + 32
        
        for k in range(count):
            if k > 0 and list_entries[k]['Adresse'] == list_entries[k-1]['Adresse']:
                continue
            current_abs_text_addr = list_entries[k]['Adresse'] + 32
            relative_offset = current_abs_text_addr - base_text_data_address_new
            
            if relative_offset < 0:
                print(f"Warning: Negative offset {relative_offset} for entry {k}")
                continue
                
            text_bytes = text_to_bytes(list_entries[k]['Texte_traduit'], is_mtp=False) + b"\x00"
            required_size = relative_offset + len(text_bytes)
            if len(new_text_block) < required_size:
                new_text_block.extend(b"\x00" * (required_size - len(new_text_block)))
            new_text_block[relative_offset:relative_offset+len(text_bytes)] = text_bytes
            
        file_bytes.extend(new_text_block)
        
        # Align POF0 to a 16-byte boundary
        marker_alignment_addr = len(file_bytes)
        padding_len = (16 - (marker_alignment_addr % 16)) % 16
        file_bytes.extend(b"\x00" * padding_len)
        
        final_pofo_addr = len(file_bytes)
        print(f"Aligned new POF0 address: 0x{final_pofo_addr:X} (padded {padding_len} bytes)")
        
        # Append POF0 and trailing blocks
        file_bytes.extend(pofo_and_rest_block)
        
        # Calculate shift size
        shift = final_pofo_addr - pofo_orig_addr
        print(f"Shift offset: {shift} bytes (0x{shift:X})")
        
        # Update header values
        struct.pack_into("<I", file_bytes, 0x04, orig_val_04 + shift)
        struct.pack_into("<I", file_bytes, 0x24, orig_val_24 + shift)
        struct.pack_into("<I", file_bytes, 0x34, orig_val_34 + shift)
        print("Header updated successfully.")
        
    else:
        # Legacy in-place compilation logic
        print("Applying in-place overwrite logic...")
        espaceprevu = sum(entry['difftaille'] for entry in list_entries)
        print(f"Expected size delta: {espaceprevu} bytes")
        
        start_addr = list_entries[0]['Adresse'] + 32
        if espaceprevu > 0:
            # Insert padding bytes
            file_bytes[start_addr:start_addr] = b"\x00" * espaceprevu
        elif espaceprevu < 0:
            # Remove excess bytes
            del file_bytes[start_addr:start_addr + abs(espaceprevu)]
            
        # Write translated strings
        for k in range(count):
            if k > 0 and list_entries[k]['Adresse'] == list_entries[k-1]['Adresse']:
                continue
            write_pos = list_entries[k]['Adresse'] + 32
            text_bytes = text_to_bytes(list_entries[k]['Texte_traduit'], is_mtp=False) + b"\x00"
            file_bytes[write_pos:write_pos + len(text_bytes)] = text_bytes
            
        print("In-place write completed.")
        
    # Write output file
    with open(output_path, "wb") as f:
        f.write(file_bytes)
    print(f"Finished writing compiled file to: {output_path}")

def extract_mxe(mxe_path, output_csv_path):
    """
    Extracts dialogue strings from an MXE file to a CSV file.
    """
    print(f"Extracting MXE: {mxe_path} to {output_csv_path}...")
    with open(mxe_path, "rb") as f:
        data = f.read()
        
    pofo_pos = data.find(b"POF0")
    if pofo_pos == -1:
        pofo_pos = len(data)
        
    # Scan for candidate pointers trying both Little Endian and Big Endian per-pointer
    candidates = []
    for ptr_offset in range(0, pofo_pos, 4):
        if ptr_offset + 4 > len(data):
            break
            
        # Try Little Endian
        val_le = struct.unpack_from("<I", data, ptr_offset)[0]
        str_addr_le = val_le + 32
        if ptr_offset < str_addr_le < pofo_pos:
            s_bytes_le = is_printable_string(data, str_addr_le)
            if s_bytes_le is not None and len(s_bytes_le) > 0:
                candidates.append((ptr_offset, val_le, str_addr_le, s_bytes_le))
                continue # If it matches LE, skip BE to avoid duplicate candidates for the same offset
                
        # Try Big Endian
        val_be = struct.unpack_from(">I", data, ptr_offset)[0]
        str_addr_be = val_be + 32
        if ptr_offset < str_addr_be < pofo_pos:
            s_bytes_be = is_printable_string(data, str_addr_be)
            if s_bytes_be is not None and len(s_bytes_be) > 0:
                candidates.append((ptr_offset, val_be, str_addr_be, s_bytes_be))
                
    if not candidates:
        print("No strings found in the file.")
        return
        
    # Get minimum text address to find the start of the text block
    min_str_addr = min(c[2] for c in candidates)
    
    # Filter candidates: keep if first string (min_str_addr) OR preceded by a null byte
    valid_strings = []
    for ptr_offset, val, str_addr, s_bytes in candidates:
        if str_addr == min_str_addr or data[str_addr - 1] == 0:
            s = bytes_to_text(s_bytes, is_mtp=False)
            valid_strings.append([ptr_offset, val, "", s, s])
            
    # Write to CSV
    with open(output_csv_path, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f, delimiter=';')
        for row in valid_strings:
            writer.writerow(row)
            
    print(f"Extracted {len(valid_strings)} strings.")

# ==============================================================================
# MTP COMPILATION AND EXTRACTION
# ==============================================================================

def compile_mtp(csv_path, template_path, output_path):
    """
    Compiles an MTP CSV file back into binary format using a template.
    """
    print(f"Compiling MTP: {csv_path} using template {template_path}...")
    
    # 1. Read CSV rows
    rows = []
    with open(csv_path, "r", encoding="utf-8", errors="ignore") as f:
        reader = csv.reader(f, delimiter=';')
        for r in reader:
            if r:
                rows.append(r)
                
    if not rows:
        raise ValueError("CSV file is empty or invalid.")
        
    list_entries = []
    for idx, row in enumerate(rows):
        if len(row) < 4:
            continue
        addr_addr = row[0].strip()
        addr = row[1].strip()
        taille_flag = int(row[2].strip())
        text_eng = row[3]
        
        # Dynamic column resolution
        text_trad = ""
        col_idx = len(row) - 1
        if col_idx >= 0 and not row[col_idx] and len(row) > 2:
            col_idx = len(row) - 2
        if col_idx >= 4:
            text_trad = row[col_idx]
            
        # Apply character replacements for MTP
        text_trad = apply_character_replacements(text_trad, is_mtp=True)
        
        list_entries.append({
            'Adresse_de_l_adresse': int(addr_addr),
            'Adresse': int(addr), # Original offset in decimal
            'Taille_texte': taille_flag,
            'Texte_anglais': text_eng,
            'Texte_traduit': text_trad,
            'difftaille': len(text_trad) - len(text_eng)
        })
        
    count = len(list_entries)
    print(f"Parsed {count} translation rows.")
    
    # Read template MTP file
    with open(template_path, "rb") as f:
        file_bytes = bytearray(f.read())
        
    # Calculate debutdutexte (start offset of the text block)
    debutdutexte = list_entries[0]['Adresse']
    
    # Calculate new offsets for each string according to alignment rules
    # Note: align using the raw CSV string length (pre-replacement) to match C++ compiler math
    current_addr = debutdutexte
    for i in range(count):
        if i == 0:
            list_entries[i]['New_Adresse'] = debutdutexte
        else:
            prev_addr = list_entries[i-1]['New_Adresse']
            prev_raw_len = len(list_entries[i-1]['Texte_traduit'])
            
            # Align next address: prev_addr + prev_raw_len + 1, aligned to 4 bytes, then add 4
            nombre = prev_addr + prev_raw_len + 1
            offset = nombre - prev_addr
            if offset % 4 != 0:
                nombre += 4 - (offset % 4)
            list_entries[i]['New_Adresse'] = nombre + 4
            
    # Calculate positionENRS (end of the new text block)
    last_addr = list_entries[-1]['New_Adresse']
    last_raw_len = len(list_entries[-1]['Texte_traduit'])
    positionENRS = last_addr + last_raw_len
    
    # Align positionENRS to 16 bytes
    if positionENRS % 16 != 0:
        positionENRS += 16 - (positionENRS % 16)
        
    print(f"Calculated text start: {debutdutexte} | text end (positionENRS): {positionENRS}")
    
    # 2. Patch relative pointers in pointer table (16-bit Little Endian)
    for entry in list_entries:
        rel_addr = entry['New_Adresse'] - debutdutexte
        ptr_offset = entry['Adresse_de_l_adresse']
        struct.pack_into("<H", file_bytes, ptr_offset, rel_addr)
        
    # 3. Reconstruct text block
    # Erase old text block (from debutdutexte - 7 to original ENRS marker)
    # To locate the original ENRS marker, search from positionENRS onwards
    orig_enrs_pos = -1
    for idx in range(debutdutexte, len(file_bytes) - 4):
        if file_bytes[idx:idx+4] == b"ENRS":
            orig_enrs_pos = idx
            break
            
    if orig_enrs_pos == -1:
        raise ValueError("ENRS marker not found in original MTP template!")
        
    # Save ENRS and trailing blocks
    enrs_and_rest_block = file_bytes[orig_enrs_pos:]
    
    # Erase old text region
    del file_bytes[debutdutexte - 7:]
    
    # Construct new zero-initialized text buffer
    text_buffer_size = positionENRS - (debutdutexte - 7)
    text_buffer = bytearray(text_buffer_size)
    
    # Write size prefixes and string data into buffer
    # The offsets are absolute, so we subtract (debutdutexte - 7) to get buffer index
    buf_offset_diff = debutdutexte - 7
    
    for entry in list_entries:
        addr = entry['New_Adresse']
        text_bytes = text_to_bytes(entry['Texte_traduit'], is_mtp=True)
        # Note: write size prefix using the CP1252 byte length after replacements
        str_len = len(text_bytes)
        
        # Buffer write offsets
        addr_buf_pos = addr - buf_offset_diff
        
        if entry['Taille_texte']:
            # Write prefix at addr_buf_pos - 4
            if str_len >= 256:
                text_buffer[addr_buf_pos - 4] = str_len & 0xFF
                text_buffer[addr_buf_pos - 3] = (str_len >> 8) & 0xFF
            else:
                text_buffer[addr_buf_pos - 4] = str_len & 0xFF
                
        # Write string bytes
        text_buffer[addr_buf_pos:addr_buf_pos + len(text_bytes)] = text_bytes
        # Null terminator is naturally there because buffer is zero-initialized
        
    # Append text buffer
    file_bytes.extend(text_buffer)
    
    # Find position of EOFC markers in the saved trailing block
    # And calculate their final offsets in file_bytes
    positionEOFC1 = 0
    positionEOFC2 = 0
    
    for idx in range(0, len(enrs_and_rest_block) - 4):
        if enrs_and_rest_block[idx:idx+4] == b"EOFC":
            current_abs = len(file_bytes) + idx
            if positionEOFC1:
                positionEOFC2 = current_abs
                break
            else:
                positionEOFC1 = current_abs
                
    # Append ENRS and trailing block
    file_bytes.extend(enrs_and_rest_block)
    
    # Patch header fields:
    # - EOFC2 at 0x04 (2 bytes LE)
    struct.pack_into("<H", file_bytes, 4, positionEOFC2)
    # - ENRS at 0x20 (2 bytes LE, stores positionENRS - 32)
    struct.pack_into("<H", file_bytes, 20, positionENRS - 32)
    
    # Write compiled file
    with open(output_path, "wb") as f:
        f.write(file_bytes)
    print(f"Finished writing compiled file to: {output_path}")

def extract_mtp(mtp_path, output_csv_path):
    """
    Extracts dialogue strings from an MTP file to a CSV file.
    """
    print(f"Extracting MTP: {mtp_path} to {output_csv_path}...")
    with open(mtp_path, "rb") as f:
        data = f.read()
        
    # Search dynamically for the pointer table and debutdutexte
    # Scan possible P_last candidates (which should be multiple of 4)
    best_candidate = None
    best_count = 0
    
    for p_last in range(0x40, len(data) - 16, 4):
        # Try both step sizes (16 and 80)
        for step in [16, 80]:
            t_start = p_last + 12 if step == 16 else p_last + 80
            if t_start >= len(data):
                continue
                
            # Scan backwards to count valid consecutive pointer records
            count = 0
            curr_p = p_last
            while curr_p >= 0x40:
                rel = struct.unpack_from("<H", data, curr_p)[0]
                str_addr = t_start + rel
                if str_addr >= len(data):
                    break
                s_bytes = is_printable_string(data, str_addr)
                if s_bytes is None:
                    break
                count += 1
                curr_p -= step
                
            if count > best_count:
                best_count = count
                best_candidate = {
                    'P_last': p_last,
                    'Step': step,
                    'T_start': t_start,
                    'Count': count
                }
                
    if not best_candidate or best_count < 3:
        print("Error: Could not dynamically detect MTP pointer table structure.")
        return
        
    p_last = best_candidate['P_last']
    step = best_candidate['Step']
    t_start = best_candidate['T_start']
    count = best_candidate['Count']
    p_start = p_last - (count - 1) * step
    
    print(f"Auto-detected MTP Pointer Table:")
    print(f"  Pointers: {p_start} -> {p_last} (step: {step}) | Count: {count}")
    print(f"  Text block starts at: {t_start}")
    
    # Extract records
    extracted_rows = []
    for i in range(count):
        ptr_offset = p_start + i * step
        rel = struct.unpack_from("<H", data, ptr_offset)[0]
        str_addr = t_start + rel
        
        s_bytes = is_printable_string(data, str_addr)
        s = bytes_to_text(s_bytes, is_mtp=True) if s_bytes is not None else ""
        
        # Check size prefix at str_addr - 4
        has_prefix = False
        L = len(s_bytes) if s_bytes else 0
        if str_addr >= 4:
            val_4 = data[str_addr - 4]
            val_3 = data[str_addr - 3]
            if L >= 256:
                if val_4 == (L & 0xFF) and val_3 == ((L >> 8) & 0xFF):
                    has_prefix = True
            else:
                if val_4 == L:
                    has_prefix = True
                    
        taille_flag = L if has_prefix else 0
        
        extracted_rows.append([ptr_offset, str_addr, taille_flag, "", s, s])
        
    # Write CSV
    with open(output_csv_path, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f, delimiter=';')
        for row in extracted_rows:
            writer.writerow(row)
            
    print(f"Extracted {len(extracted_rows)} strings.")

# ==============================================================================
# MASSIVE EXTRACTION AND COMPILATION
# ==============================================================================

def mass_extract(src_dir, dest_dir):
    """
    Mass extracts all .mxe and .mtp files in a folder to CSVs.
    """
    print(f"Mass Extract: scanning {src_dir} for binaries...")
    os.makedirs(dest_dir, exist_ok=True)
    
    mxe_files = glob.glob(os.path.join(src_dir, "*.mxe"))
    mtp_files = glob.glob(os.path.join(src_dir, "*.mtp"))
    
    print(f"Found {len(mxe_files)} MXE files and {len(mtp_files)} MTP files.")
    
    for f in mxe_files:
        base = os.path.splitext(os.path.basename(f))[0]
        csv_out = os.path.join(dest_dir, f"{base}.csv")
        try:
            extract_mxe(f, csv_out)
        except Exception as e:
            print(f"[ERROR] Extracting {f}: {e}")
            
    for f in mtp_files:
        base = os.path.splitext(os.path.basename(f))[0]
        csv_out = os.path.join(dest_dir, f"{base}.csv")
        try:
            extract_mtp(f, csv_out)
        except Exception as e:
            print(f"[ERROR] Extracting {f}: {e}")
            
    print("Mass extraction completed.")

def mass_compile(csv_dir, template_dir, dest_dir):
    """
    Mass compiles all CSV files in csv_dir to binaries, using templates from template_dir.
    Template and CSV can also be in the same folder (pass the same path for both).
    Format is auto-detected from the template file extension (.mxe or .mtp).
    Output files go to dest_dir (created automatically if missing).
    """
    print(f"Mass Compile: scanning {csv_dir} for CSV files...")
    print(f"  Templates from: {template_dir}")
    print(f"  Output to:      {dest_dir}")
    os.makedirs(dest_dir, exist_ok=True)
    
    csv_files = glob.glob(os.path.join(csv_dir, "*.csv"))
    print(f"Found {len(csv_files)} CSV files.")
    
    ok = 0
    errors = 0
    skipped = 0
    
    for csv_file in csv_files:
        base = os.path.splitext(os.path.basename(csv_file))[0]
        
        # Auto-detect format from the template file extension
        mxe_template = os.path.join(template_dir, f"{base}.mxe")
        mtp_template = os.path.join(template_dir, f"{base}.mtp")
        
        if os.path.exists(mxe_template):
            out_bin = os.path.join(dest_dir, f"{base}_new.mxe")
            try:
                compile_mxe(csv_file, mxe_template, out_bin, base)
                ok += 1
            except Exception as e:
                print(f"[ERROR] Compiling MXE {base}: {e}")
                errors += 1
        elif os.path.exists(mtp_template):
            out_bin = os.path.join(dest_dir, f"{base}_new.mtp")
            try:
                compile_mtp(csv_file, mtp_template, out_bin)
                ok += 1
            except Exception as e:
                print(f"[ERROR] Compiling MTP {base}: {e}")
                errors += 1
        else:
            print(f"[SKIP] No template found for '{base}' (expected {base}.mxe or {base}.mtp)")
            skipped += 1
            
    print(f"\nMass compilation completed: {ok} OK, {errors} errors, {skipped} skipped.")

# ==============================================================================
# MAIN ENTRY POINT
# ==============================================================================

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Valkyria Chronicles Translation Tool -- Gestisce file MXE e MTP",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Struttura cartelle consigliata:
  Patch/Script/
  |- valkyria_tool.py
  |- mxe/               <- template .mxe + CSV tradotti nello stesso posto
  |   |- *.mxe
  |   +- *.csv
  +- mtp/
      |- templates/     <- file .mtp originali del gioco
      +- csv/           <- CSV tradotti

Esempi d'uso:
  # Estrae tutte le stringhe da una cartella di binari
  python valkyria_tool.py extract-all --src ./mxe --dest ./mxe

  # Compila tutti i CSV usando i template nella stessa cartella (output -> mxe/compilati/)
  python valkyria_tool.py compile-all --dir ./mxe

  # Compila con cartelle separate per template e CSV (MTP)
  python valkyria_tool.py compile-all --dir ./mtp/templates --csv-dir ./mtp/csv

  # Specifica anche la destinazione output
  python valkyria_tool.py compile-all --dir ./mxe --dest ./output/mxe

  # Singolo file (formato auto-rilevato dall'estensione del template)
  python valkyria_tool.py compile --template orig.mxe --csv trad.csv --output nuovo.mxe
  python valkyria_tool.py extract --input file.mxe --output out.csv
"""
    )
    subparsers = parser.add_subparsers(dest="command", help="Comando")
    
    # 1. extract -- estrae stringhe da un singolo binario
    extract_parser = subparsers.add_parser("extract", help="Estrae stringhe da un file binario")
    extract_parser.add_argument("--input", required=True, help="File .mxe o .mtp di input")
    extract_parser.add_argument("--output", required=True, help="File .csv di output")
    extract_parser.add_argument("--format", choices=["mxe", "mtp"],
        help="Formato (opzionale, rilevato automaticamente dall'estensione del file)")
    
    # 2. compile -- compila un singolo CSV in binario
    compile_parser = subparsers.add_parser("compile", help="Compila un CSV in formato binario")
    compile_parser.add_argument("--template", required=True, help="File template .mxe o .mtp originale")
    compile_parser.add_argument("--csv", required=True, help="File .csv di traduzione")
    compile_parser.add_argument("--output", required=True, help="File di output compilato")
    compile_parser.add_argument("--format", choices=["mxe", "mtp"],
        help="Formato (opzionale, rilevato automaticamente dall'estensione del template)")
    
    # 3. extract-all -- estrae tutti i binari da una cartella
    extract_all_parser = subparsers.add_parser("extract-all",
        help="Estrae tutte le stringhe dai file .mxe/.mtp in una cartella")
    extract_all_parser.add_argument("--src", required=True,
        help="Cartella sorgente con i file .mxe/.mtp")
    extract_all_parser.add_argument("--dest", required=True,
        help="Cartella di destinazione per i .csv generati")
    
    # 4. compile-all -- compila tutti i CSV in una cartella
    compile_all_parser = subparsers.add_parser("compile-all",
        help="Compila tutti i CSV in una cartella usando i template")
    compile_all_parser.add_argument("--dir", required=True,
        help="Cartella con i template .mxe/.mtp (e i .csv se nello stesso posto)")
    compile_all_parser.add_argument("--csv-dir", default=None, dest="csv_dir",
        help="Cartella separata per i .csv (opzionale, default: stessa di --dir)")
    compile_all_parser.add_argument("--dest", default=None,
        help="Cartella di output (opzionale, default: sottocartella 'compilati' dentro --dir)")
    
    args = parser.parse_args()
    
    # --- extract ---
    if args.command == "extract":
        fmt = args.format
        if fmt is None:
            ext = os.path.splitext(args.input)[1].lower()
            fmt = "mtp" if ext == ".mtp" else "mxe"
            print(f"Formato rilevato automaticamente: {fmt.upper()}")
        if fmt == "mxe":
            extract_mxe(args.input, args.output)
        else:
            extract_mtp(args.input, args.output)
    
    # --- compile ---
    elif args.command == "compile":
        fmt = args.format
        if fmt is None:
            ext = os.path.splitext(args.template)[1].lower()
            fmt = "mtp" if ext == ".mtp" else "mxe"
            print(f"Formato rilevato automaticamente: {fmt.upper()}")
        base_name = os.path.splitext(os.path.basename(args.template))[0]
        if fmt == "mxe":
            compile_mxe(args.csv, args.template, args.output, base_name)
        else:
            compile_mtp(args.csv, args.template, args.output)
    
    # --- extract-all ---
    elif args.command == "extract-all":
        mass_extract(args.src, args.dest)
    
    # --- compile-all ---
    elif args.command == "compile-all":
        template_dir = args.dir
        csv_dir      = args.csv_dir if args.csv_dir else args.dir
        dest_dir     = args.dest    if args.dest    else os.path.join(args.dir, "compilati")
        mass_compile(csv_dir, template_dir, dest_dir)
    
    else:
        parser.print_help()
        sys.exit(1)
