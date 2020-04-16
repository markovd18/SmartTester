# SmartTester
## Spuštění ve Visual Studiu
Pro spuštění ve vývojovém prostředí je nutné mít nainstalovaný `CMake`. Jeho aktuální vezre se dá stáhnout [zde](https://cmake.org/download/).
Je třeba nainstalovat 64 bitovou verzi. Nejpohodlnější je stáhnout instalátor `.msi`.
Po instalaci je třeba spustit `CMake GUI`, zde nastavit cestu ke zdrojovým kódům, tj. cestu k CMakeLists.txt v projektu a složku, 
do které se přeloží binárka, ideálně do složky /bin v rootu projektu. Pak stačí kliknout na `Configure`, kde je třeba vybrat 
`Visual Studio 16 2019` jako generátor a `x64` platformu, zbytek může zůstat tak, jak je a kliknout na `Finish`. Pak stačí jen vybrat 
`Generate` a projekt bude vygenerovaný. Ve Visual Studiu pak stačí otevřít CMake, vybrat CMakeLists.txt v rootu projektu a projekt 
se otevře.
