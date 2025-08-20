
ORG 0x7c00 ; bios looks here for boot sector
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start: ; we need this to load from bios, first 2 bytes is short jump, then no op (bios signiture)
    jmp short start
    nop
times 33 db 0 ; the total size is 33 bytes, go to osdev.org/FAT to see why

start:
    jmp 0:step2 ; ensureing that we are actually starting at 0x0000

step2:
    cli ; clear Interrupts
    mov ax, 0x00 ; data segment start
    mov ds, ax
    mov es, ax
    mov ss, ax ; ss is stack segment
    mov sp, 0x7c00 ; stack pointer now at 
    sti ; enables interrupts

.load_protected:
    cli
    lgdt[gdt_descriptor] ; load global descripter table
    mov eax, cr0
    or eax, 0x1 ; ensure 1 is in the lowest bit to enable protected mode
    mov cr0, eax ;enable protected mode
    jmp CODE_SEG:load32
   


; gdt, look at osdev to see. we will use default values for the bit fields
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:     ; CS should point to this
    dw 0xffff ; segment limit first 0-15 bits
    dw 0      ; base first 15 bits
    db 0      ; base 16-23 bits
    db 0x9a   ; access byte
    db 11001111b ; high 4 bit flags and the low 4 bit flags
    db 0      ; base 24-31 bits

; offset 0x10
gdt_data:     ; DS, SS, ES, FS, GS
    dw 0xffff ; segment limit first 0-15 bits
    dw 0      ; base first 15 bits
    db 0      ; base 16-23 bits
    db 0x92   ; access byte
    db 11001111b ; high 4 bit flags and the low 4 bit flags
    db 0

gdt_end: 

gdt_descriptor:
    dw gdt_end - gdt_start-1 ; size of descriptor
    dd gdt_start

[BITS 32]
load32:
    mov eax, 1 ; sector to start from, 0 is the bootloader
    mov ecx, 100 ; writing 100 sectors for kernel
    mov edi, 0x0100000 ; 1 Megabyte
    call ata_lva_read ; talk with the driver and load sectors into memory
    jmp CODE_SEG:0x0100000

ata_lva_read:
    mov ebx, eax ; backup the LBA
    ; send the highest 8 bits of the lba to hard disk controller
    shr eax, 24 ; shift by 24 bits to only have 8
    or eax, 0xE0 ; selct the master drive (slave/master drive) 
    mov dx, 0x1F6 ; Port to write to, it expects it
    out dx, al 
    ; finished sending the highest 8 bits of the lba

    ; send total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; finished sending

    ; send more bits of the LBA
    mov eax, ebx ; restore the backup of the LBA
    mov dx, 0x1F3
    out dx, al 
    ; finished sending more bits

    ; sending more bits
    mov dx, 0x1F4
    mov eax, ebx 
    shr eax, 8
    out dx, al
    ; finished sending bits

    ; send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, ebx
    shr eax, 16
    out dx, al
    ; finished

    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

    ; read all sectors into memory
.next_sector:
    push ecx ; use later

; checking if we need to read
.try_again:
    mov dx, 0x1F7 ;read from this port into al
    in al, dx
    test al, 8 
    jz .try_again ; keep reading 8 bits until it is just 8 secters

; we need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1F0
    rep insw ; reads word from I/O port specified in the dx register into memory location specified in the ES:(E)DI, 0x1F0 -> 0x0100000
    pop ecx
    loop .next_sector ; mov to the next sector, decrement ecx
    ; end of reading sectors into memory
    ret

times 510-($ - $$) db 0 ; making the file 512 bytes no matter what, boot signature
dw 0xAA55 ; little endian so we are putting it backwards

