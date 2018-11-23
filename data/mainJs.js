$(document).ready(function () {
  $(".noClick").on("click",function(e){
    e.preventDefault(); 
  })
  var m = {};
   
  //--- msg------------
  m.msg = function (t) {
    alert(t);
    console.log(t);
    $("#logTxt").text("log:" + t);
  };
  //=======msg=========
  m.log=function(t){
    console.log(t);
    $("#logTxt").text("log:" + t);
  }
  m.showStatus=function(s){
    $("#statusText").html(s);
  }
  //--BTN TEST PHONE NOMB----
  $("#btnDialTel").on("click", function(e){
    var tel=$("#userTel").val();
    m.log("Викликаю: +"+tel);
    $.get("/dial",{"tel":tel})
    .done(function(d){
     m.log("Все ок! Дозвонились!"+d) ;
    })
    .fail(function(){
      m.log("Не вдалося дозвонитись");
    })
  })
  
  //------get system status 
  m.getStatus = function (){
    $.getJSON("/sysStatus")
    .done(function (dd) {
      var st="";
      if (dd.isUsersInHome){
        st+="Охорона вимкнена!<br>";
        st+="в домі:<br>"
        dd.activUser.forEach(function(el) {
         st+=el+"<br>";
      });
      }else{st="ОХОРОНЯЮ!"}
      m.showStatus(st);
      
    })
    .fail(function (e) {
      m.msg("Помилка отримання статусу системи!\n");
      m.showStatus("Статус не отримано!")
    });
  }
  //=========================
  //-----getUser------
  m.getUser = function getUser(userId) {
    var p = {};
    p.userId = userId;
    $.getJSON("/getUser", p)
      .done(function (d) {
        $('#userId').val(d.userId);
        $('#userName').val(d.userName);
        $('#userTel').val(d.userTel);
        $('#userMac').val(d.userMac);

      })
      .fail(function () {
        m.msg("Помилка завантаження даних користувача!")
      })
  };
  //======================
 // ------ INIT -----------
 m.getUser(0); //-----init form
 m.getStatus();
 //======= INIT============

  // --- save USER----------
  $('#frmUser').submit(function (event) {
    event.preventDefault();
    if (!confirm("Надати доступ до будинку користувачу?")) return;
    var g = $(this).serialize();
    $.ajax({
        method: "POST",
        url: "/setUser",
        cache: false,
        data: g
      })
      .done(function (dd) {
        m.msg("Збережено!\n" + dd);
      })
      .fail(function (e) {
        m.msg("Помилка збереження!\n");
      });
  });

//====================================

  $('.btnLastNext').on('click', function (e) {

    e.preventDefault();
    var btnClick = $(this).attr("id")
    i = $("#userId").val();
    if (i < 0 || i > 10) {
      alert("Не допустимі значення!");
      return;
    }
    if (btnClick == "btnNext") { i++;} else { i--;}
    m.getUser(i);
    console.log("Click: " + $(this).text());
  });

  //=====end of doc redy========
});

//-----------------menu--------------------
(function ($) {
  $(function () {
    $('.menu__icon').on('click', function () {
      $(this).closest('.menu')
        .toggleClass('menu_state_open');
    });
    $('.menu__links-item').on('click', function (e) {
      e.preventDefault();
      console.log("Click: " + $(this).text());
      console.log(e);
      $(this).closest('.menu')
        .removeClass('menu_state_open');
    });
    $("#btnTest").on("click", function () {
      var f = $("#frmUser");
      if (f.css("display") == "none") {
        f.show(500);
      } else {
        f.hide(500);
      }
    });
  });
})(jQuery);