
void __scroll(char param_1,char param_2,char param_3,char param_4,char param_5,char param_6)

{
  byte bVar1;
  byte bVar2;
  undefined2 unaff_SS;
  undefined1 local_a2 [160];
  
  if (((DAT_4125_3e7b == '\0') && (DAT_4125_3e81 != 0)) && (param_1 == '\x01')) {
    param_5 = param_5 + '\x01';
    bVar1 = param_4 + 1;
    param_3 = param_3 + '\x01';
    bVar2 = param_2 + 1;
    if (param_6 == '\x06') {
      movetext(param_5,bVar1 + 1,param_3,bVar2,param_5,bVar1);
      gettext(param_5,bVar2,param_5,bVar2,local_a2);
      __blank_cells(param_3,param_5,local_a2,unaff_SS);
    }
    else {
      movetext(param_5,bVar1,param_3,bVar2 - 1,param_5,bVar1 + 1);
      gettext(param_5,bVar1,param_5,bVar1,local_a2);
      __blank_cells(param_3,param_5,local_a2,unaff_SS);
      bVar2 = bVar1;
    }
    puttext(param_5,bVar2,param_3,bVar2,local_a2);
  }
  else {
    __video_int();
  }
  return;
}


