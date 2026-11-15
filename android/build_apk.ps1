# Build the Marauder Remote WebView APK with the Android build-tools (no Gradle).
$ErrorActionPreference = "Stop"
$JAVA = "C:\Program Files\Microsoft\jdk-17.0.21.21-hotspot"
$SDK  = "D:\sp32\android\sdk"
$BT   = "$SDK\build-tools\33.0.2"
$AJAR = "$SDK\platforms\android-33\android.jar"
$PROJ = "D:\sp32\apk"
$OUT  = "$PROJ\out"

Remove-Item -Recurse -Force $OUT -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force "$OUT\classes" | Out-Null

Write-Host "1/7 aapt2 link (package manifest)..."
& "$BT\aapt2.exe" link -o "$OUT\base.apk" -I $AJAR --manifest "$PROJ\AndroidManifest.xml" --min-sdk-version 21 --target-sdk-version 33

Write-Host "2/7 javac..."
& "$JAVA\bin\javac.exe" -source 8 -target 8 -bootclasspath $AJAR -d "$OUT\classes" "$PROJ\src\com\marauder\remote\MainActivity.java"

Write-Host "3/7 d8 (dex)..."
$classFiles = @(Get-ChildItem -Recurse "$OUT\classes\*.class" | ForEach-Object { $_.FullName })
$d8args = @('-cp', "$BT\lib\d8.jar", 'com.android.tools.r8.D8', '--min-api', '21', '--lib', $AJAR, '--output', $OUT) + $classFiles
& "$JAVA\bin\java.exe" @d8args
if (-not (Test-Path "$OUT\classes.dex")) { throw "d8 failed: classes.dex not produced" }

Write-Host "4/7 add classes.dex into apk..."
Copy-Item "$OUT\base.apk" "$OUT\unsigned.apk" -Force
& "$JAVA\bin\jar.exe" uf "$OUT\unsigned.apk" -C $OUT classes.dex

Write-Host "5/7 zipalign..."
& "$BT\zipalign.exe" -f 4 "$OUT\unsigned.apk" "$OUT\aligned.apk"

Write-Host "6/7 debug keystore..."
if (-not (Test-Path "$OUT\debug.keystore")) {
  & "$JAVA\bin\keytool.exe" -genkeypair -v -keystore "$OUT\debug.keystore" -storepass android -keypass android -alias androiddebugkey -keyalg RSA -keysize 2048 -validity 10000 -dname "CN=Android Debug,O=Android,C=US"
}

Write-Host "7/7 apksigner sign..."
& "$BT\apksigner.bat" sign --ks "$OUT\debug.keystore" --ks-pass pass:android --key-pass pass:android --out "$PROJ\MarauderRemote.apk" "$OUT\aligned.apk"

Write-Host "=== DONE ==="
& "$BT\apksigner.bat" verify --print-certs "$PROJ\MarauderRemote.apk" | Select-Object -First 2
Get-Item "$PROJ\MarauderRemote.apk" | ForEach-Object { "APK: $($_.FullName)  ($([math]::Round($_.Length/1KB,1)) KB)" }
