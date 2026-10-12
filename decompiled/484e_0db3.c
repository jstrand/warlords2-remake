
void __cdecl16far diplomacy_apply(void)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char *fmt;
  undefined4 uVar6;
  
  iVar3 = current_player;
  iVar5 = 0;
  do {
    if (7 < iVar5) {
      return;
    }
    if ((iVar5 != iVar3) &&
       ((*(byte *)(iVar3 * 8 + iVar5 + 0x153b) & 3) !=
        (*(byte *)(iVar3 * 8 + iVar5 + 0x153b) & 0xc) >> 2)) {
      if ((*(byte *)(iVar3 * 8 + iVar5 + 0x153b) & 3) <
          (*(byte *)(iVar3 * 8 + iVar5 + 0x153b) & 0xc) >> 2) {
        bVar2 = *(byte *)(iVar3 * 8 + iVar5 + 0x153b);
        iVar4 = iVar3 * 8 + iVar5;
        pbVar1 = (byte *)(iVar4 + 0x153b);
        *pbVar1 = *pbVar1 & 0xfc;
        pbVar1 = (byte *)(iVar4 + 0x153b);
        *pbVar1 = *pbVar1 | (byte)((bVar2 & 0xc) >> 2);
        bVar2 = *(byte *)(iVar3 * 8 + iVar5 + 0x153b);
        iVar4 = iVar5 * 8 + iVar3;
        pbVar1 = (byte *)(iVar4 + 0x153b);
        *pbVar1 = *pbVar1 & 0xfc;
        pbVar1 = (byte *)(iVar4 + 0x153b);
        *pbVar1 = *pbVar1 | (byte)((bVar2 & 0xc) >> 2);
        if ((*(byte *)(iVar5 * 8 + iVar3 + 0x153b) & 0xc) >> 2 <
            (*(byte *)(iVar5 * 8 + iVar3 + 0x153b) & 3)) {
          bVar2 = *(byte *)(iVar5 * 8 + iVar3 + 0x153b);
          iVar4 = iVar5 * 8 + iVar3;
          pbVar1 = (byte *)(iVar4 + 0x153b);
          *pbVar1 = *pbVar1 & 0xf3;
          pbVar1 = (byte *)(iVar4 + 0x153b);
          *pbVar1 = *pbVar1 | (bVar2 & 3) << 2;
        }
        if ((((*(byte *)(iVar3 * 8 + iVar5 + 0x153b) & 0xc) == 8) &&
            (FUN_6d51_1244(iVar3,9,iVar3,iVar5,0), DAT_3c04_0159 != 0)) &&
           (iVar4 = is_computer_turn(), iVar4 != 0)) {
          iVar4 = 1;
LAB_484e_0f47:
          uVar6 = CONCAT22(0x3c04,iVar5 * 0x14);
          fmt = get_string(0x6f,iVar4);
          sprintf(&DAT_451b_1d28,fmt,uVar6);
          FUN_8065_11d1();
        }
      }
      else if ((*(byte *)(iVar5 * 8 + iVar3 + 0x153b) & 0xc) >> 2 <=
               (*(byte *)(iVar3 * 8 + iVar5 + 0x153b) & 0xc) >> 2) {
        bVar2 = *(byte *)(iVar3 * 8 + iVar5 + 0x153b);
        iVar4 = iVar3 * 8 + iVar5;
        pbVar1 = (byte *)(iVar4 + 0x153b);
        *pbVar1 = *pbVar1 & 0xfc;
        pbVar1 = (byte *)(iVar4 + 0x153b);
        *pbVar1 = *pbVar1 | (byte)((bVar2 & 0xc) >> 2);
        bVar2 = *(byte *)(iVar3 * 8 + iVar5 + 0x153b);
        iVar4 = iVar5 * 8 + iVar3;
        pbVar1 = (byte *)(iVar4 + 0x153b);
        *pbVar1 = *pbVar1 & 0xfc;
        pbVar1 = (byte *)(iVar4 + 0x153b);
        *pbVar1 = *pbVar1 | (byte)((bVar2 & 0xc) >> 2);
        if ((((*(byte *)(iVar3 * 8 + iVar5 + 0x153b) & 0xc) == 0) &&
            (FUN_6d51_1244(), DAT_3c04_0159 != 0)) && (iVar4 = is_computer_turn(), iVar4 != 0)) {
          iVar4 = 0;
          goto LAB_484e_0f47;
        }
      }
    }
    iVar5 = iVar5 + 1;
  } while( true );
}


