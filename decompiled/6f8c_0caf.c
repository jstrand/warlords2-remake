
undefined2 __cdecl16far FUN_6f8c_0caf(void)

{
  int *piVar1;
  int iVar2;
  int unaff_BP;
  int unaff_DI;
  undefined2 unaff_SS;
  
  if (*(int *)(unaff_BP + 8) != 0) {
    *(undefined2 *)(unaff_BP + -10) = 0;
    while ((*(int *)(unaff_BP + -0xc) == 0 && (*(int *)(unaff_BP + -10) < 0x14))) {
      iVar2 = dice(1,3,-2);
      *(int *)*(undefined4 *)(unaff_BP + 10) = *(int *)(unaff_DI * 0x41 + 0x157d) + iVar2;
      iVar2 = dice(1,3,-2);
      piVar1 = (int *)*(undefined4 *)(unaff_BP + 0xe);
      *piVar1 = *(int *)(unaff_DI * 0x41 + 0x157f) + iVar2;
      *(uint *)(unaff_BP + -0xc) =
           (uint)((*(byte *)(*piVar1 * 0xe0 + *(int *)*(undefined4 *)(unaff_BP + 10) * 2 + 1) & 0x10
                  ) == 0);
      *(int *)(unaff_BP + -10) = *(int *)(unaff_BP + -10) + 1;
    }
  }
  return *(undefined2 *)(unaff_BP + -0xc);
}


