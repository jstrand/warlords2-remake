
void __cdecl16far FUN_740d_0bad(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  int local_4;
  
  armies_at_tile(param_1,param_2,(int *)CONCAT22(unaff_SS,&local_4));
  if (local_4 != 0) {
    FUN_1b62_09ec(local_4,0);
    iVar1 = DAT_451b_052a + 0x10;
    iVar3 = (((DAT_451b_0528 + (int)DAT_451b_052c / 2) - (local_4 * 0x18) / 2) + -0xc >> 3) << 3;
    for (iVar2 = 0; iVar2 < local_4; iVar2 = iVar2 + 1) {
      uVar4 = (undefined2)((ulong)*(undefined4 *)((undefined2 *)&DAT_451b_1ebe + iVar2 * 2) >> 0x10)
      ;
      FUN_8611_08be((int)*(char *)(((undefined2 *)&DAT_451b_1ebe)[iVar2 * 2] + 4),
                    (int)*(char *)(((undefined2 *)&DAT_451b_1ebe)[iVar2 * 2] + 5),0x342a,0x4125,
                    iVar3,iVar1,0,(int)*(char *)(((undefined2 *)&DAT_451b_1ebe)[iVar2 * 2] + 7),
                    (int)*(char *)((int)*(undefined4 *)((undefined2 *)&DAT_451b_1ebe + iVar2 * 2) +
                                  8),0,0);
      iVar3 = iVar3 + 0x18;
    }
  }
  return;
}


