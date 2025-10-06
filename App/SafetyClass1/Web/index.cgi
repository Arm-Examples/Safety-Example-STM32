t <html><head><title>Safety application example - Traffic Light</title>
t   <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t   <script language=JavaScript type="text/javascript">
t     var formUpdate = new periodicObj("refresh.cgx");
t     function updateDynamicElements() {
t       var mode = document.getElementById("mode_id");
t       if (mode.value != undefined) {
t         if (mode.value == 0) {                // If normal mode then display "Normal" in green
t           mode.style.color = "green";
t           mode.innerText   = "Normal";
t         } else {                              // If safe mode then display "Safe" in red
t           mode.style.color = "red";
t           mode.innerText   = "Safe";
t         }
t       }
t       var light_red = document.getElementById("light_red_id");
t       if (light_red.value != undefined) {
t         if (light_red.value == '1') {         // If red light is on
t           light_red.style.backgroundColor = "red";
t         } else {                              // If red light is off
t           light_red.style.backgroundColor = "black";
t         }
t       }
t       var light_yellow = document.getElementById("light_yellow_id");
t       if (light_yellow.value != undefined) {
t         if (light_yellow.value == '1') {      // If yellow light is on
t           light_yellow.style.backgroundColor = "yellow";
t         } else {                              // If yellow light is off
t           light_yellow.style.backgroundColor = "black";
t         }
t       }
t       var light_green = document.getElementById("light_green_id");
t       if (light_green.value != undefined) {
t         if (light_green.value == '1') {       // If green light is on
t           light_green.style.backgroundColor = "green";
t         } else {                              // If green light is off
t           light_green.style.backgroundColor = "black";
t         }
t       }
t       var sensor_val = document.getElementById("sensor_val_id");
t       if (sensor_val.value != undefined) {
t         sensor_val.style.width = (((sensor_val.value * 100) / 4096) + '%');
t       }
t       var fault_info = document.getElementById("fault_info_id");
t       if (fault_info.value != undefined) {
t         fault_info.innerHTML  = fault_info.value;
t       } else {
t         fault_info.innerHTML  = " ";
t       }
t     }
t 
t     window.addEventListener('DOMContentLoaded', function() {
t       updateMultiple(formUpdate, updateDynamicElements);
t       setInterval(function(){ updateMultiple(formUpdate, updateDynamicElements); }, 250);
t       updateDynamicElements();
t     });
t   </script>
t   <style>
t     .mode              { font-size: 28; color: green; }
t     .light_red         { height: 25px; width: 25px; background-color:transparent;
t                          border-radius: 50%; display: inline-block; }
t     .light_yellow      { height: 25px; width: 25px; background-color:transparent;
t                          border-radius: 50%; display: inline-block; }
t     .light_green       { height: 25px; width: 25px; background-color:transparent;
t                          border-radius: 50%; display: inline-block; }
t     input[type=submit] { text-align: center; font-size: 20; height: 32px; width: 90% }
t     td                 { text-align: center; text-align: center; }
t     form               { margin: 0; top: 50%; left: 50%; }
t   </style>
t   </head>
t   <body bgColor=#ffffff leftMargin=0 topMargin=10 marginwidth="0" marginheight="0">
t     <div align=center style="width: 1000; height: 470">
t     <table style="border: 1px solid #000080" height=384 cellSpacing=0 cellPadding=0 width="1000">
t     <tbody>
t     <tr bgColor=#EEEEEE>
t      <td style="border-bottom: 1px solid #000080" vAlign=bottom noWrap height=70 margin=50 width="800">
t       <h2 align="center"><font face="verdana" color="#006699">Safety application example - Traffic light</font></h2>
t      </td>
t      <td style="border-bottom: 1px solid #000080" vAlign=center noWrap height=73 width="170">
t       <a href="http://www.keil.com"><img border="0" src="Arm_KEIL.png" align="middle" width="225" height="35"></a>
t      </td>
t     </tr>
t     <tr>
t       <td colSpan=5 height=380 width="1000">
t       <div align=center>
t         <center><font face="verdana">
t           <table width="95%" border=0 align=center>
t             <tbody><tr>
t             <td width="95%">
t               <table width="100%" border=0 align=center>
t                 <tr style="background-color: transparent">
t                   <th width=30%></th>
t                   <th width=35%></th>
t                   <th width=35%></th>
t                   <tr style="font-weight: bold; height: 4em">
t                     <td style="font-size: 28; text-align:left">Operating mode</td>
t                     <td><span id="mode_id" class="mode"></span></td>
t                   </tr>
t                   <tr>
t                     <th rowspan="3" style="font-weight: bold; font-size: 28; text-align:left; height: 4em">Traffic light</th>
t                     <td><span id="light_red_id" class="light_red"></span></td>
t                   </tr>
t                   <tr>
t                     <td><span id="light_yellow_id" class="light_yellow"></span></td>
t                   </tr>
t                   <tr>
t                     <td><span id="light_green_id" class="light_green"></span></td>
t                   </tr>
t                   <tr style="font-weight: bold; height: 4em">
t                     <td style="font-size: 28; text-align:left;">Traffic sensor</td>
t                     <td height=50>
t                       <table bgcolor="#FFFFFF" border="2" cellpadding="0" cellspacing="0" width="100%">
t                         <tr>
t                           <td>
t                             <table id="sensor_val_id" style="width: 0%" border="0" cellpadding="0" cellspacing="0">
t                               <tr>
t                                 <td bgcolor="#0000FF">&nbsp;</td>
t                               </tr>
t                             </table>
t                           </td>
t                         </tr>
t                       </table>
t                     </td>
t                   </tr>
t                   <tr style="font-weight: bold; height: 4em">
t                     <td style="font-size: 28; text-align:left;">Inject fault</td>
t                   </tr>
t                   <tr style="height: 5em; vertical-align:top;">
t                     <td style="font-size: 24; text-align:left;">&nbsp;&nbsp;Thread watchdog</td>
t                     <td>
t                       <form action="" method="post">
t                         <input type=submit name=Btn0;
t                         value="OperationVerificationThread";
t                         title="Lock execution in OperationVerificationThread to trigger its thread watchdog.&#10
t                         The thread watchdog handler transitions to safe operation mode and suspends low safety threads.";/>
t                         <br>
t                         <label for="Btn0" style="font-size: 16;">safety class 0</label>
t                       </form>
t                     </td>
t                     <td>
t                       <form action="" method="post">
t                         <input type=submit name=Btn1;
t                         value="SafeModeOperationThread";
t                         title="Lock execution in SafeModeOperationThread to trigger its thread watchdog.&#10
t                         The thread watchdog handler considers this as a fatal error and resets the system.";/>
t                         <br>
t                         <label for="Btn1" style="font-size: 16;">safety class 2</label>
t                       </form>
t                     </td>
t                   </tr>
t                   <tr style="height: 6em; vertical-align:top;">
t                     <td style="font-size: 24; text-align:left;">&nbsp;&nbsp;Memory access</td>
t                     <td>
t                       <form action="" method="post">
t                         <input type=submit name=Btn2;
t                         value="NormalOperationThread";
t                         title="Access memory outside of the MPU Protected Zone assigned to NormalOperationThread.&#10
t                         This triggers MemManage_Handler that terminates operation for this Zone.";/>
t                         <br>
t                         <label for="Btn2" style="font-size: 16;">MPU Protected Zone 0<br>safety class 0</label>
t                       </form>
t                     </td>
t                     <td>
t                       <form action="" method="post">
t                         <input type=submit name=Btn3;
t                         value="CommunicationInitThread";
t                         title="Access memory outside of the MPU Protected Zone assigned to CommunicationInitThread.&#10
t                         This triggers MemManage_Handler that terminates operation for this Zone.";/>
t                         <br>
t                         <label for="Btn3" style="font-size: 16;">MPU Protected Zone 2<br>safety class 1</label>
t                       </form>
t                     </td>
t                   </tr>
t                   <tr style="height: 4em; vertical-align:top;">
t                     <td style="font-size: 24; text-align:left;">&nbsp;&nbsp;Fatal</td>
t                     <td>
t                       <form action="" method="post">
t                         <input type  =submit name=Btn5;
t                         value="Suspend kernel";
t                         title="Suspend RTOS to emulate system lockup.&#10
t                         A chain of unrecoverable thread watchdog alarms triggers HW watchdog and system reset.";/>
t                       </form>
t                     </td>
t                   </tr>
t                   <tr style="height: 4em">
t                     <td style="font-size: 24; text-align:left; vertical-align:top">&nbsp;&nbsp;Fault info</td>
t                     <td colspan="2">
t                       <form">
t                         <textarea id="fault_info_id" rows="10" cols="75" disabled readonly style="font-size: 16"></textarea>
t                       </form>
t                     </td>
t                   </tr>
t                 </tr></tbody>
t               </table>
t             <tbody><tr>
t         </tr></tbody></table>
t         </font></center>
t         </div>
t       </td>
t       </tr><tr>
t       <td colSpan=5 height=20 width="1000">
t       <p align=center><font face="Verdana" size="1"><a href="https://www.arm.com/company/policies/terms-and-conditions">
t         Copyright</a> &#169; 2021-2025 Arm Limited. All rights reserved.</font></p>
t       </td>
t     </tr></tbody></table>
t     </div>
t   </body>
t </html>
. <! -- End of script must be closed with period -->
