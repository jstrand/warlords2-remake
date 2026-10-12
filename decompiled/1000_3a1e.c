
void __bios_text_write(int param_1,undefined2 *param_2,undefined2 *param_3)

{
  undefined2 uVar1;
  undefined2 unaff_SS;
  bool bVar2;
  bool bVar3;
  undefined2 local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  local_8 = __wherexy();
  bVar2 = param_3._2_2_ == DAT_4125_3e7f;
  if (bVar2) {
    local_4 = __screen_rowcol((undefined2 *)param_3,param_3._2_2_);
  }
  bVar3 = param_2._2_2_ == DAT_4125_3e7f;
  if (bVar3) {
    local_6 = __screen_rowcol((undefined2 *)param_2,param_2._2_2_);
  }
  while (param_1 != 0) {
    if (bVar3) {
      __advance_cursor(&local_8,unaff_SS,&local_6,unaff_SS);
      uVar1 = __video_int();
    }
    else {
      uVar1 = *param_2;
      param_2 = (undefined2 *)CONCAT22(param_2._2_2_,(undefined2 *)param_2 + 1);
    }
    if (bVar2) {
      __advance_cursor(&local_8,unaff_SS,&local_4,unaff_SS);
      __video_int();
      param_1 = param_1 + -1;
    }
    else {
      *param_3 = uVar1;
      param_3 = (undefined2 *)CONCAT22(param_3._2_2_,(undefined2 *)param_3 + 1);
      param_1 = param_1 + -1;
    }
  }
  __video_int();
  return;
}


