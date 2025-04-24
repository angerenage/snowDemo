import os
import sys

def sanitize_symbol(name):
	return name.replace('.', '_').replace('-', '_').replace('/', '_').replace('\\', '_')

def embed_file(fpath, root_dir):
	with open(fpath, "rb") as f:
		content = f.read()

	file_name = os.path.basename(fpath)
	sym_name = sanitize_symbol(file_name)

	data_lines = []
	for i in range(0, len(content), 12):
		chunk = content[i:i+12]
		line = ", ".join(f"0x{b:02x}" for b in chunk) + ","
		data_lines.append("\t" + line)

	return {
		"name": file_name,
		"symbol": sym_name,
		"size": len(content),
		"data": "\n".join(data_lines)
	}

def generate(files, out_c, out_h):
	with open(out_c, "w") as fc, open(out_h, "w") as fh:
		# HEADER
		fh.write("#pragma once\n\n")
		fh.write("#include <stddef.h>\n\n")
		fh.write("typedef struct {\n\tvoid* data;\n\tsize_t size;\n} Ressource;\n\n")

		# IMPLEMENTATION
		fc.write(f'#include "{os.path.basename(out_h)}"\n\n')

		for res in files:
			# Header declaration
			fh.write(f"extern const Ressource res_{res['symbol']};\n")

			# Implementation
			fc.write(f"static const unsigned char res_{res['symbol']}_data[] = {{\n{res['data']}\n}};\n\n")
			fc.write(f"const Ressource res_{res['symbol']} = {{ (void*)res_{res['symbol']}_data, {res['size']} }};\n\n")

def main():
	if len(sys.argv) < 4:
		print("Usage: resources_embedder.py <output.c> <output.h> <file1> [file2 ...]")
		sys.exit(1)

	out_c = sys.argv[1]
	out_h = sys.argv[2]
	file_paths = sys.argv[3:]

	# Find common prefix to act as "root"
	root_dir = os.path.commonpath(file_paths)

	files = [embed_file(path, root_dir) for path in file_paths]

	generate(files, out_c, out_h)

if __name__ == "__main__":
	main()
