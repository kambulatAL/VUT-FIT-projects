<?php
declare(strict_types=1);

$x = 5;
$y = 10;

if ($x === $y) {
    $x = $x + $y;
    return $x;
} else {
    if ($x >= $y) {
        $x = $x - $y;
        return $x;
    } else {
        $x = $x * $y;
        return $x;
    } 
}

?>