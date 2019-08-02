const char MAIN_page[] PROGMEM = R"=====(
<html lang="en"><head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <meta name="description" content="">
    <meta name="author" content="">
    <link rel="icon" href="https://getbootstrap.com/docs/4.0/examples/sign-in/docs/4.0/assets/img/favicons/favicon.ico">

    <title>Signin</title>

    <link rel="canonical" href="https://getbootstrap.com/docs/4.0/examples/sign-in/">

    <!-- Bootstrap core CSS -->
    <link href="https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/css/bootstrap.min.css"
    rel="stylesheet" id="bootstrap-css">
    <link href="https://cdn.jsdelivr.net/gh/gitbrent/bootstrap4-toggle@3.5.0/css/bootstrap4-toggle.min.css" rel="stylesheet">

    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.1.1/js/bootstrap.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
    <script src="https://cdn.jsdelivr.net/gh/gitbrent/bootstrap4-toggle@3.5.0/js/bootstrap4-toggle.min.js"></script>

    <!-- Custom styles for this template -->
    <!-- <link href="signin.css" rel="stylesheet"> -->
  </head>

  <body class="text-center">

    <style media="screen">
      html,
      body {
        height: 100%;
      }

      body {
        display: -ms-flexbox;
        display: -webkit-box;
        display: flex;
        -ms-flex-align: center;
        -ms-flex-pack: center;
        -webkit-box-align: center;
        align-items: center;
        -webkit-box-pack: center;
        justify-content: center;
        padding-top: 40px;
        padding-bottom: 40px;
        background-color: #f5f5f5;
      }

      .form-signin {
        width: 100%;
        max-width: 330px;
        padding: 15px;
        margin: 0 auto;
      }
      .form-signin .checkbox {
        font-weight: 400;
      }
      .form-signin .form-control {
        position: relative;
        box-sizing: border-box;
        height: auto;
        padding: 10px;
        font-size: 16px;
      }
      .form-signin .form-control:focus {
        z-index: 2;
      }
      .form-signin input[type="email"] {
        margin-bottom: -1px;
        border-bottom-right-radius: 0;
        border-bottom-left-radius: 0;
      }
      .form-signin input[type="password"] {
        margin-bottom: 10px;
        border-top-left-radius: 0;
        border-top-right-radius: 0;
      }
    </style>

    <content>
      <row>
        <img class="mb-4" src="https://getbootstrap.com/docs/4.0/assets/brand/bootstrap-solid.svg" alt="" width="72" height="72">
      </row>
      <row>
        <h1 class="h3 mb-3 font-weight-normal">Кухня. Основное освещение.</h1>
      </row>
      <row>
        <input id="toggle-event" type="checkbox" data-toggle="toggle">
      </row>
      <row>
        <div id="console-event"></div>
      </row>
    </content>
    <script>
    $(function() {
      $('#toggle-event').change(function() {
        $('#console-event').html('Toggle: ' + $(this).prop('checked'))
          if ( $(this).prop('checked') ) {
              console.log("checked");
              $.post(
                  "/api/v1/state/switch",
                  {
                      state: "on"
                  }
              );

          } else {
              console.log("unchecked");
              $.post(
                  "/api/v1/state/switch",
                  {
                      state: "off"
                  }
              );
          }
      })
    })
    </script>
</body></html>
)=====";
