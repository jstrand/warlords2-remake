
void __cdecl16far FUN_8611_0238(int param_1,int param_2)

{
  int iVar1;
  undefined1 local_a [8];
  
  if ((((DAT_3c04_0177 <= param_1) && (param_1 <= DAT_3c04_017b)) && (DAT_3c04_0175 <= param_2)) &&
     (((param_2 <= DAT_3c04_0179 && (DAT_3c04_0159 != 0)) &&
      ((DAT_4125_3254 == 0 ||
       (iVar1 = FUN_8611_1055(DAT_3c04_017d,DAT_3c04_017f,0), *(char *)(iVar1 + 0x2d56) != '\x0e')))
      ))) {
    FUN_8611_0053();
    FUN_216d_002e(local_a);
    FUN_24d0_009d(0x6795,0x4125,local_a);
  }
  return;
}


