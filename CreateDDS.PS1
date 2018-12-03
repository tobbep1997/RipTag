$files = Get-ChildItem "Assets" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name
	
	
	Assets/texconv.exe $name -o $dir -m 1 -f DXT5 -y -w 512	-h 512
}

$files = Get-ChildItem "Assets/GUIFOLDER" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name
	
	
	Assets/texconv.exe $name -o $dir -m 1 -f RGBA -y 
}
