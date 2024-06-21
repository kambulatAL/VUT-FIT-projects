<?php
declare(strict_types=1);
// Program 2: Vypocet faktorialu (rekurzivne)
// Hlavni telo programu
write("Zadejte cislo pro vypocet faktorialu: "); $a = readi();
// Definice funkce pro vypocet hodnoty faktorialu
function factorial(int $n) : int { if ($n < 2) {
$result = 1; } else {
$decremented_n =  $n - 1; factorial($decremented_n); 
$temp_result = $result = $n *$temp_result;
}
return $result;
}
?>