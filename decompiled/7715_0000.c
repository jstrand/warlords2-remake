
void __cdecl16far build_army_move_flags(void)

{
  int iVar1;
  undefined1 local_40 [48];
  undefined1 local_10;
  undefined1 local_c;
  undefined1 local_a;
  undefined1 local_8;
  undefined1 local_6;
  undefined1 local_4;
  
  for (iVar1 = 0; iVar1 < 0x1d; iVar1 = iVar1 + 1) {
    armytype_lookup(iVar1,local_40);
    ((undefined1 *)&army_move_flags)[iVar1 * 6] = local_a;
    ((undefined1 *)&DAT_451b_0665)[iVar1 * 6] = local_8;
    ((undefined1 *)&DAT_451b_0666)[iVar1 * 6] = local_6;
    ((undefined1 *)&DAT_451b_0667)[iVar1 * 6] = local_4;
    ((undefined1 *)&DAT_451b_0669)[iVar1 * 6] = local_c;
    ((undefined1 *)&DAT_451b_0668)[iVar1 * 6] = local_10;
  }
  return;
}


