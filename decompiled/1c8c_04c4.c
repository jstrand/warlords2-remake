
void __cdecl16far FUN_1c8c_04c4(void)

{
  int iVar1;
  int in_DX;
  undefined2 uVar2;
  undefined4 local_6;
  
  if (DAT_451b_1f02 == 0) goto LAB_1c8c_0513;
  if (*(int *)((int)DAT_451b_1f02 + 0x12) < 0) goto LAB_1c8c_0513;
  do {
    uVar2 = (undefined2)((ulong)DAT_451b_1f02 >> 0x10);
    auto_tutorial_hook_2
              (*(undefined2 *)((int)DAT_451b_1f02 + 0x12),*(undefined2 *)((int)DAT_451b_1f02 + 0x14)
              );
LAB_1c8c_0513:
    do {
      iVar1 = FUN_8c07_05c4();
      local_6 = CONCAT22(in_DX,iVar1);
      if (iVar1 == 0 && in_DX == 0) {
        FUN_8065_0174();
        return;
      }
      *(uint *)(iVar1 + 0xc) = *(uint *)(iVar1 + 0xc) | 0x200;
    } while (*(int *)(iVar1 + 0x12) < 0);
    FUN_8c07_06eb(local_6);
    FUN_89e0_0356();
  } while( true );
}


