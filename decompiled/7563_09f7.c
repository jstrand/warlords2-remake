
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_7563_09f7(void)

{
  byte *pbVar1;
  char cVar2;
  int iVar3;
  int *piVar4;
  undefined2 uVar5;
  
  uVar5 = (undefined2)((ulong)_DAT_451b_1efe >> 0x10);
  cVar2 = *(char *)(*(byte *)(((int *)_DAT_451b_1efe)[1] * 0xe0 + *_DAT_451b_1efe * 2) + 0x710);
  if ((((_DAT_451b_1efe != (int *)0x0) && (cVar2 != '\x02')) && (cVar2 != '\x03')) &&
     (((cVar2 != '\n' && (cVar2 != '\v')) &&
      ((*(byte *)(((int *)_DAT_451b_1efe)[1] * 0x70 + *_DAT_451b_1efe) & 0x40) == 0)))) {
    list_carried_items(2,0,_DAT_451b_1efe);
    for (iVar3 = 0; iVar3 < DAT_451b_08ce; iVar3 = iVar3 + 1) {
      if (((undefined2 *)&DAT_451b_0c9c)[iVar3] == current_player) {
        piVar4 = (int *)_DAT_451b_1efe;
        *(int *)(((undefined2 *)&DAT_451b_0c9c)[iVar3] * 0x1d + 0xd02) = *_DAT_451b_1efe;
        *(int *)(((undefined2 *)&DAT_451b_0c9c)[iVar3] * 0x1d + 0xd04) = piVar4[1];
        *(undefined1 *)(((undefined2 *)&DAT_451b_0c9c)[iVar3] * 0x1d + 0xcff) = 1;
        *(undefined2 *)(((undefined2 *)&DAT_451b_0c9c)[iVar3] * 0x1d + 0xd00) = 0;
        pbVar1 = (byte *)(((int *)_DAT_451b_1efe)[1] * 0x70 + *_DAT_451b_1efe);
        *pbVar1 = *pbVar1 | 0x40;
        FUN_8611_006c();
        break;
      }
    }
    FUN_8065_0174();
  }
  return;
}


