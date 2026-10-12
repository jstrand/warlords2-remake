
void __cdecl16far FUN_6a35_041c(void)

{
  int iVar1;
  int iVar2;
  
  FUN_54f6_0000(8);
  DAT_451b_0406 = 0x12a;
  if (DAT_451b_029a == '\x0f') {
    iVar1 = 8;
  }
  else {
    iVar1 = (int)DAT_451b_029a;
  }
  if (DAT_451b_029b == '\x0f') {
    iVar2 = 8;
  }
  else {
    iVar2 = (int)DAT_451b_029b;
  }
  FUN_1997_034a(0x26);
  FUN_22bf_07a5();
  (*DAT_451b_2a68)(0x22bf,0x53bc,0x4125,iVar2 << 5,0,0x240020,0x342a,0x4125,DAT_4125_0d3c,0x5381,
                   0x4125,iVar2 << 5,0);
  (*DAT_451b_2a68)(0x22bf,0x53bc,0x4125,iVar1 << 5,0,0x240020,0x342a,0x4125,DAT_4125_0d40,0x5381,
                   0x4125,iVar1 << 5,0);
  FUN_22bf_0757();
  return;
}


