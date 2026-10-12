
void __cdecl16far auto_ui_random_world(void)

{
  undefined2 unaff_SS;
  char *fmt;
  char local_20 [30];
  
  FUN_24d0_0002();
  FUN_216d_0288(0x2940);
  if (DAT_4125_28c8 == 0) {
    strcpy((char *)CONCAT22(unaff_SS,local_20),(char *)0x41255d80);
  }
  else {
    fmt = get_string(0,0);
    sprintf((char *)CONCAT22(unaff_SS,local_20),fmt);
  }
  FUN_78a8_06ae(0xf0002,0x30000);
  FUN_7ecb_00d6(DAT_4125_293c,local_20);
  return;
}


