import argparse
import json
import os
import re

script_dir = os.path.dirname(os.path.abspath(__file__))
src_dir = os.path.join(script_dir, '..', 'src')

def scan_files():
    # Matches LOCSTR("...") with the argument in capture group 1.
    pattern = re.compile(r'LOCSTR\(\s*"([^"]+)"\s*\)')
    keys = set()
    for root, _, files in os.walk(src_dir):
        for file in files:
            if file.endswith(('.cpp', '.hpp', '.h')):
                path = os.path.join(root, file)
                try:
                    with open(path, 'r', encoding='utf-8') as f:
                        content = f.read()
                    for match in pattern.finditer(content):
                        key = match.group(1).strip()
                        if key:
                            keys.add(key)
                except Exception as e:
                    print(f"Error reading {path}: {e}")
    return keys

def update_translation_file(lang, keys):
    filename = os.path.join(src_dir, 'assets', f'translation.{lang}.json')
    # Load existing translations if file exists
    if os.path.exists(filename):
        with open(filename, 'r', encoding='utf-8') as f:
            try:
                translations = json.load(f)
            except json.JSONDecodeError:
                print(f"Warning: {filename} is not valid JSON. Starting fresh.")
                translations = {}
    else:
        translations = {}

    # Only add missing keys with empty translation.
    did_update = False
    for key in keys:
        if key not in translations:
            did_update = True
            if lang == "en":
                translations[key] = key
            else:
                translations[key] = ""

    with open(filename, 'w', encoding='utf-8') as f:
        json.dump(translations, f, indent=2, ensure_ascii=False)
    if did_update:
        print(f"Updated {filename} with keys missing translations.")
    else:
        print(f"No updates needed for {filename}.")

def main():
    parser = argparse.ArgumentParser(
        description="Scan for LOCSTR macros and update translation files for the specified languages."
    )
    parser.add_argument(
        "--languages",
        nargs="+",
        required=True,
        help="List of language codes (e.g., la en) to update translation files for."
    )

    args = parser.parse_args()
    keys = scan_files()
    if not keys:
        print("No LOCSTR macros found.")
        return

    for lang in args.languages:
        update_translation_file(lang, keys)

if __name__ == '__main__':
    main()