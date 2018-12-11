#If you can not run this script
	#please use the command
	#Set-ExecutionPolicy Unrestricted
		#Then enter A

$MIP_MAP_LEVEL = 1
$COMPRESSION_TYPE = 'BC7_UNORM'

#-----------------------------------------------------------------	HIGH
#$files = Get-ChildItem "Assets" -include *.png -Recurse -Force
#$files | Foreach-Object  {
#	$name = $_.FullName
#	$dir = Split-Path -Path $name	
	
#	Assets/texconv.exe $name -o $dir -sx _1024 -pow2 -m $MIP_MAP_LEVEL -f $COMPRESSION_TYPE -y -w 1024	-h 1024
#}
#$files = Get-ChildItem "Assets" -include *.jpg -Recurse -Force
#$files | Foreach-Object  {
#	$name = $_.FullName
#	$dir = Split-Path -Path $name	
	
#	Assets/texconv.exe $name -o $dir -sx _1024 -pow2 -m $MIP_MAP_LEVEL -f $COMPRESSION_TYPE -y -w 1024	-h 1024
#}

#-----------------------------------------------------------------	MEDUIM
$files = Get-ChildItem "Assets" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name
	
	Assets/texconv.exe $name -o $dir -sx _512 -pow2 -m $MIP_MAP_LEVEL -f $COMPRESSION_TYPE -y -w 512	-h 512
}
$files = Get-ChildItem "Assets" -include *.jpg -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -sx _512 -pow2 -m $MIP_MAP_LEVEL -f $COMPRESSION_TYPE -y -w 512	-h 512
}

#-----------------------------------------------------------------	LOW
$files = Get-ChildItem "Assets" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name
	
	Assets/texconv.exe $name -o $dir -sx _256 -pow2 -m $MIP_MAP_LEVEL -f $COMPRESSION_TYPE -y -w 256	-h 256
}
$files = Get-ChildItem "Assets" -include *.jpg -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -sx _256 -pow2 -m $MIP_MAP_LEVEL -f $COMPRESSION_TYPE -y -w 256	-h 256
}


#-----------------------------------------------------------------
$files = Get-ChildItem "Assets/GUIFOLDER" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -pow2 -m $MIP_MAP_LEVEL -f $COMPRESSION_TYPE -y 
}
$files = Get-ChildItem "Assets/GUIFOLDER" -include *.jpg -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -pow2 -m $MIP_MAP_LEVEL -f $COMPRESSION_TYPE -y 
}
#-----------------------------------------------------------------
$files = Get-ChildItem "Assets/PAUSEFOLDER" -include *.png -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -pow2 -m $MIP_MAP_LEVEL -f $COMPRESSION_TYPE -y 
}
$files = Get-ChildItem "Assets/PAUSEFOLDER" -include *.jpg -Recurse -Force
$files | Foreach-Object  {
	$name = $_.FullName
	$dir = Split-Path -Path $name	
	
	Assets/texconv.exe $name -o $dir -pow2 -m $MIP_MAP_LEVEL -f $COMPRESSION_TYPE -y 
}