; Autor reseni: Kambulat Alakaev xalaka00

; Projekt 2 - INP 2022
; Vernamova sifra na architekture MIPS64

; DATA SEGMENT
                .data
login:          .asciiz "xalaka00"  ; sem doplnte vas login
cipher:         .space  17  ; misto pro zapis sifrovaneho loginu

params_sys5:    .space  8   ; misto pro ulozeni adresy pocatku
                            ; retezce pro vypis pomoci syscall 5
                            ; (viz nize "funkce" print_string)

; CODE SEGMENT
                .text

                ; ZDE NAHRADTE KOD VASIM RESENIM
main:

                
                daddi r10, r0, 0            ; x   a  l   a  k   a 00
                lb r25, login(r10)          ; +1 -12 +1 -12 +1 -12  yomolo
                daddi r27,r0,0

        mloop:
                slti r27, r25, 97
                bne r0, r27, end
                
                j operate1


        operate1:
                daddi r28,r25,1
                slti r27, r28,123
                beq r0, r27, bigger
                
                sb r28, cipher(r10)
                daddi r10,r10,1
                lb r25, login(r10)

                slti r27, r25, 97
                bne r0, r27, end

                j operate2

        operate2:
                daddi r28,r25,-12
                slti r27,r28,97
                bne r0, r27,lower

                sb r28,cipher(r10)
                daddi r10,r10,1
                lb r25, login(r10)

                j mloop

        bigger:
                daddi r28,r28,-122
                daddi r28,r28,96

                sb r28, cipher(r10)
                daddi r10,r10,1
                lb r25, login(r10)

                slti r27, r25, 97
                bne r0, r27, end

                j operate2

        lower:
                daddi r28,r28,-97
                daddi r28,r28,123

                sb r28, cipher(r10)
                daddi r10,r10,1
                lb r25, login(r10)

                j mloop

        end:
                daddi r4, r0, cipher
                jal     print_string    ; vypis pomoci print_string - viz nize


                syscall 0   ; halt

print_string:   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address
