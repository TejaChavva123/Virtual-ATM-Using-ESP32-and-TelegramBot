var sheet_id = "1-JCss_sKPSfB8c3lfuG2ZU8Tj67szyzoh_WOvriT1OE";
var sheet_name1 = "credentials";
var sheet_name2 = "Transactions";
var ss = SpreadsheetApp.openById(sheet_id);
var sheet1 = ss.getSheetByName(sheet_name1);
var sheet2 = ss.getSheetByName(sheet_name2);

function doGet(e){
  // for getting details of the credit,debit,mini statement.
  if (e.parameter.transaction){
    var data1 = Number(e.parameter.transaction);
    var data2 = Number(e.parameter.balance);
    if (data1>0){
      sheet2.appendRow([data2-data1,data1,data2,"CREDITED"+String(data1)]);
    }
    else if (data1==0){
      sheet2.appendRow([data2,data1,data2,"NO ACTION PERFORMED"]);
    }
    else{
      sheet2.appendRow([data2-data1,data1,data2,"DEBITED"+String(data2)]);
    }
    
    return;
  }
  // password update.
  if (e.parameter.passwordUpdate){
    var data4 = e.parameter.passwordUpdate;
    sheet1.getRange("B1").setValue(data4);
    return;
  }
  // final balance update.
  if (e.parameter.finalbalance){
    var data3 = Number(e.parameter.finalbalance);
    var getblock = sheet1.getRange("D2");
    getblock.setValue(Number(data3));
    sheet2.getRange("H2").setValue(Number(data3));
    return;
  }
  // fetching the username, password, balance.
  if (e.parameter.read){
    var read = e.parameter.read;
    return ContentService.createTextOutput(sheet1.getRange(read).getValue());

  }
