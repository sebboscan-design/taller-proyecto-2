# Corre los 8 casos de uso desde cero y guarda las salidas en pruebas/
# Uso (desde la carpeta del proyecto, ya compilado taller.exe):
#   powershell -ExecutionPolicy Bypass -File pruebas\correr_pruebas.ps1

# se empieza con los archivos vacios
Remove-Item -Recurse -Force datos -ErrorAction SilentlyContinue
Get-ChildItem -Directory -Filter "backup_*" | Remove-Item -Recurse -Force

$antes = "1a","1b","2","3a","3b","3c","4a","4b","5a","5b","6","7a"
foreach ($caso in $antes) {
    Get-Content "pruebas\entrada_$caso.txt" | .\taller.exe | Out-File -Encoding utf8 "pruebas\salida_$caso.txt"
    Write-Host "Caso $caso listo"
}

# caso 7: se reemplaza vehiculos.bin por la copia del backup
$backup = Get-ChildItem -Directory -Filter "backup_*" | Select-Object -First 1
Copy-Item "$($backup.FullName)\vehiculos.bin" "datos\vehiculos.bin" -Force
Write-Host "datos\vehiculos.bin reemplazado con $($backup.Name)\vehiculos.bin"

foreach ($caso in "7b","8") {
    Get-Content "pruebas\entrada_$caso.txt" | .\taller.exe | Out-File -Encoding utf8 "pruebas\salida_$caso.txt"
    Write-Host "Caso $caso listo"
}
Write-Host "Listo. La carpeta datos quedo con los datos pre-poblados."
