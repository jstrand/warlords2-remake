
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_8611_1972(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  undefined4 local_1c;
  undefined4 local_14;
  undefined4 local_c;
  undefined4 local_8;
  
  if ((DAT_451b_2509 & 1) != 0) {
    if ((DAT_451b_226b != DAT_451b_1fc5) || (_DAT_451b_226d != DAT_451b_1fc7)) {
      uVar4 = DAT_451b_226b - DAT_451b_1fc5 >> 0xf;
      if (1 < (int)((DAT_451b_226b - DAT_451b_1fc5 ^ uVar4) - uVar4)) {
        return;
      }
      uVar4 = _DAT_451b_226d - DAT_451b_1fc7 >> 0xf;
      if (1 < (int)((_DAT_451b_226d - DAT_451b_1fc7 ^ uVar4) - uVar4)) {
        return;
      }
      FUN_828e_0b51(_DAT_451b_1fc5,_DAT_451b_226b);
      FUN_8611_3080();
    }
    for (iVar3 = 0; iVar3 < 9; iVar3 = iVar3 + 1) {
      for (iVar5 = 0; iVar5 < 9; iVar5 = iVar5 + 1) {
        iVar1 = iVar3 * 0x48 + iVar5 * 8;
        local_8 = (byte *)CONCAT22(0x4125,(byte *)(iVar1 + 0x5f3b));
        iVar2 = iVar3 * 0x48 + iVar5 * 8;
        local_c = (byte *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_2281 + iVar2);
        if ((*local_c & 1) != 0) {
          local_14 = (long *)CONCAT22(0x4125,(long *)(iVar1 + 0x5f3f));
          local_1c = (long *)CONCAT22(0x4125,(long *)(iVar2 + 0x61e5));
          if ((*local_14 == *local_1c) && (*(long *)local_8 == *(long *)local_c)) {
            *local_8 = *local_8 & 0xfe;
          }
        }
      }
    }
  }
  return;
}


