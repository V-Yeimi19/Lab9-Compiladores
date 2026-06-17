import os
import subprocess
import shutil

# Archivos c++
programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp"]

# Compilar
compile = ["g++"] + programa
print("Compilando:", " ".join(compile))
result = subprocess.run(compile, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación:\n", result.stderr)
    exit(1)

print("Compilación exitosa")

# Ejecutar
input_dir = "inputs"
output_dir = "outputs"
os.makedirs(output_dir, exist_ok=True)

for i in range(1, 11):
    filename = f"input{i}.txt"
    filepath = os.path.join(output_dir, filename)

    if os.path.isfile(filepath):
        continue

    print(f"Ejecutando {filename}")
    result = subprocess.run(["./a.out", filepath], capture_output=True, text=True)
