#-----------------------------------------------------------------	ULTRA
$files = Get-ChildItem "Assets" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -sx _2048 -m 1 -f DXT5 -y -w 2048	-h 2048
}
$files = Get-ChildItem "Assets" -include *.jpg -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -sx _2048  -m 1 -f DXT5 -y -w 2048	-h 2048
}

#-----------------------------------------------------------------	HIGH
$files = Get-ChildItem "Assets" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -sx _1024 -m 1 -f DXT5 -y -w 1024	-h 1024
}
$files = Get-ChildItem "Assets" -include *.jpg -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -sx _1024 -m 1 -f DXT5 -y -w 1024	-h 1024
}

#-----------------------------------------------------------------	MEDUIM
$files = Get-ChildItem "Assets" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name
	
	Assets/texconv.exe $name -o $dir -sx _512 -m 1 -f DXT5 -y -w 512	-h 512
}
$files = Get-ChildItem "Assets" -include *.jpg -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -sx _512 -m 1 -f DXT5 -y -w 512	-h 512
}

#-----------------------------------------------------------------	LOW
$files = Get-ChildItem "Assets" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name
	
	Assets/texconv.exe $name -o $dir -sx _256 -m 1 -f DXT5 -y -w 256	-h 256
}
$files = Get-ChildItem "Assets" -include *.jpg -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -sx _256 -m 1 -f DXT5 -y -w 256	-h 256
}


#-----------------------------------------------------------------
$files = Get-ChildItem "Assets/GUIFOLDER" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -m 1 -f BGRA -y 
}
$files = Get-ChildItem "Assets/GUIFOLDER" -include *.jpg -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -m 1 -f BGRA -y 
}
#-----------------------------------------------------------------
$files = Get-ChildItem "Assets/PAUSEFOLDER" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -m 1 -f BGRA -y 
}
$files = Get-ChildItem "Assets/PAUSEFOLDER" -include *.jpg -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -m 1 -f BGRA -y 
}