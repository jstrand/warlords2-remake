
void __cdecl16near __scan_next_arg(void)

{
  int unaff_BP;
  undefined2 unaff_SS;
  
  if ((*(byte *)(unaff_BP + -1) & 0x20) != 0) {
    *(int *)(unaff_BP + 0x10) = *(int *)(unaff_BP + 0x10) + 4;
    return;
  }
  *(int *)(unaff_BP + 0x10) = *(int *)(unaff_BP + 0x10) + 2;
  return;
}


