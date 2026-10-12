
void __cdecl16far diplomacy_score_update(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  iVar2 = current_player;
  iVar4 = 0;
  do {
    if (7 < iVar4) {
      return;
    }
    if (((iVar4 != iVar2) &&
        ((*(byte *)(iVar2 * 8 + iVar4 + 0x153b) & 0xc) >> 2 <
         (*(byte *)(iVar2 * 8 + iVar4 + 0x153b) & 3))) &&
       ((*(byte *)(iVar2 * 8 + iVar4 + 0x153b) & 0xc) >> 2 <
        (*(byte *)(iVar4 * 8 + iVar2 + 0x153b) & 0xc) >> 2)) {
      if (((*(byte *)(iVar2 * 8 + iVar4 + 0x153b) & 0xc) == 0) &&
         ((*(byte *)(iVar2 * 8 + iVar4 + 0x153b) & 3) == 1)) {
LAB_484e_1194:
        iVar3 = dice(1,2,0);
        iVar3 = iVar3 + 1;
      }
      else {
        if (((*(byte *)(iVar2 * 8 + iVar4 + 0x153b) & 0xc) != 0) ||
           ((*(byte *)(iVar2 * 8 + iVar4 + 0x153b) & 3) != 2)) {
          if (((*(byte *)(iVar2 * 8 + iVar4 + 0x153b) & 0xc) != 4) ||
             ((*(byte *)(iVar2 * 8 + iVar4 + 0x153b) & 3) != 2)) goto LAB_484e_11b0;
          goto LAB_484e_1194;
        }
        iVar3 = dice(1,10,0);
        iVar3 = iVar3 + 10;
      }
      piVar1 = (int *)(iVar2 * 2 + 0x10e3);
      *piVar1 = *piVar1 + iVar3;
    }
LAB_484e_11b0:
    iVar4 = iVar4 + 1;
  } while( true );
}


