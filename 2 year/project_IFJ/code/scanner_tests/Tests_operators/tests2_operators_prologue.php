<?php
declare(strict_types=1);

$x = 40;
$y = 3;

if ($x !== $y) {
    $x = $x . $y;
    return $x;
} else {
    if ($x < $y) {
        $x = $x + $y;
        return $x;
    } else {
        $x = $x - $y;
        return $x;
    } 
}

$arr = "sada";
?>