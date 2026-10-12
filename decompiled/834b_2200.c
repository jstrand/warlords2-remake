
void __cdecl16far FUN_834b_2200(void)

{
  undefined2 uVar1;
  undefined2 in_DX;
  int iVar2;
  int iVar3;
  
  if (opt_hidden_map != 0) {
    if ((DAT_451b_158d != 0) && ((DAT_451b_15b8 & 10) == 0)) {
      for (iVar2 = 0; iVar2 < (int)(uint)DAT_451b_15b7; iVar2 = iVar2 + 1) {
        uVar1 = mem_lock(((undefined4 *)&DAT_451b_158d)[iVar2]);
        ((undefined2 *)&DAT_451b_159f)[iVar2 * 2] = in_DX;
        ((undefined2 *)&DAT_451b_159d)[iVar2 * 2] = uVar1;
      }
    }
    FUN_2012_07cc(0x54e3,0x4125,1);
    for (iVar2 = 0; iVar2 < 0x70; iVar2 = iVar2 + 1) {
      for (iVar3 = 0; iVar3 < 0x9c; iVar3 = iVar3 + 1) {
        if ((*(byte *)(iVar3 * 0x70 + iVar2) & 0x20) != 0) {
          (*DAT_451b_2a70)(0x2012,0x54e3,0x4125,iVar2 << 1,iVar3 << 1,0);
          (*DAT_451b_2a70)(0x2012,0x54e3,0x4125,iVar2 * 2 + 1,iVar3 << 1);
          (*DAT_451b_2a70)(0x2012,0x54e3,0x4125,iVar2 << 1,iVar3 * 2 + 1);
          (*DAT_451b_2a70)(0x2012,0x54e3,0x4125,iVar2 * 2 + 1);
        }
      }
    }
    if ((DAT_451b_158d != 0) && ((DAT_451b_15b8 & 10) == 0)) {
      for (iVar2 = 0; iVar2 < (int)(uint)DAT_451b_15b7; iVar2 = iVar2 + 1) {
        mem_unlock(((undefined4 *)&DAT_451b_158d)[iVar2]);
      }
    }
  }
  return;
}


