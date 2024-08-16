.global song
.global song_size
.section .rodata
song:
    .incbin "C:/Users/jmateo/ENSC452/piano_tiles/Software/piano_tiles_core1/src/sources/audio_binaries/song.bin"
    .align 2
1:
song_size:
    .int 1b - song


.global drum
.global drum_size
.section .rodata
drum:
    .incbin "C:/Users/jmateo/ENSC452/piano_tiles/Software/piano_tiles_core1/src/sources/audio_binaries/drum.bin"
    .align 2
2:
drum_size:
    .int 2b - drum


.global piano
.global piano_size
.section .rodata
piano:
    .incbin "C:/Users/jmateo/ENSC452/piano_tiles/Software/piano_tiles_core1/src/sources/audio_binaries/piano.bin"
	.align 2
3:
piano_size:
    .int 3b - piano


.global chime
.global chime_size
.section .rodata
chime:
    .incbin "C:/Users/jmateo/ENSC452/piano_tiles/Software/piano_tiles_core1/src/sources/audio_binaries/chime.bin"
    .align 2
4:
chime_size:
    .int 4b - chime

.global boing
.global boing_size
.section .rodata
boing:
	.incbin "C:/Users/jmateo/ENSC452/piano_tiles/Software/piano_tiles_core1/src/sources/audio_binaries/boing.bin"
	.align 2
5:
boing_size:
    .int 5b - boing
