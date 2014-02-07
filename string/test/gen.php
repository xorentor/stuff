<?php
function RandomString( $len )
{
    $characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    $randstring = "";
    for ($i = 0; $i < $len; $i++) {
        $randstring .= $characters[rand(0, strlen($characters)-1)];
    }
    return $randstring;
}

if( !isset($argv[ 1 ]) && empty($argv[ 1 ]) ) {
	printf( "missing argument\n" );
	exit( 0 );
}

$fh = fopen( $argv[ 1 ], "w");

for( $i = 0; $i < 1024; $i++ ) {
	$randstring = RandomString( $argv[ 1 ] );
	fwrite( $fh, $randstring."\n" );
}
