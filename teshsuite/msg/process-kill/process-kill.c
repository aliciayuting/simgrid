/* Copyright (c) 2007, 2009-2017. The SimGrid Team. All rights reserved.    */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include "simgrid/msg.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(msg_process_kill, "Messages specific for this msg example");

static int victimA_fun(XBT_ATTRIB_UNUSED int argc, XBT_ATTRIB_UNUSED char* argv[])
{
  XBT_INFO("Hello!");
  XBT_INFO("Suspending myself");
  MSG_process_suspend(MSG_process_self()); /* - First suspend itself */
  XBT_INFO("OK, OK. Let's work");          /* - Then is resumed and start to execute a task */
  MSG_task_execute(MSG_task_create("work", 1e9, 0, NULL));
  XBT_INFO("Bye!"); /* - But will never reach the end of it */
  return 0;
}

static int victimB_fun(XBT_ATTRIB_UNUSED int argc, XBT_ATTRIB_UNUSED char* argv[])
{
  XBT_INFO("Terminates before being killed");
  return 0;
}

static int killer(XBT_ATTRIB_UNUSED int argc, XBT_ATTRIB_UNUSED char* argv[])
{
  XBT_INFO("Hello!"); /* - First start a victim process */
  msg_process_t victimA = MSG_process_create("victim A", victimA_fun, NULL, MSG_host_by_name("Fafard"));
  msg_process_t victimB = MSG_process_create("victim B", victimB_fun, NULL, MSG_host_by_name("Bourassa"));
  MSG_process_sleep(10.0);

  XBT_INFO("Resume victimA"); /* - Resume it from its suspended state */
  MSG_process_resume(victimA);

  XBT_INFO("Kill victimA"); /* - and then kill it */
  MSG_process_kill(victimA);

  XBT_INFO("Kill victimB, even if it's already dead"); /* that's a no-op, there is no zombies in SimGrid */
  MSG_process_kill(victimB);

  XBT_INFO("OK, goodbye now. I commit a suicide.");
  MSG_process_kill(MSG_process_self());

  XBT_INFO("This line will never get displayed: I'm already dead since the previous line.");
  return 0;
}

int main(int argc, char* argv[])
{
  MSG_init(&argc, argv);
  xbt_assert(argc == 2, "Usage: %s platform_file\n\tExample: %s msg_platform.xml\n", argv[0], argv[0]);

  MSG_create_environment(argv[1]); /* - Load the platform description */
  /* - Create and deploy killer process, that will create the victim process  */
  MSG_process_create("killer", killer, NULL, MSG_host_by_name("Tremblay"));

  msg_error_t res = MSG_main(); /* - Run the simulation */

  XBT_INFO("Simulation time %g", MSG_get_clock());
  return res != MSG_OK;
}