(function ($) {


  /**
   * Add an extra function to the Drupal ajax object
   * which allows us to trigger an ajax response without
   * an element that triggers it.
   */
  Drupal.ajax.prototype.specifiedResponse = function() {
    var ajax = this;
 
    // Do not perform another ajax command if one is already in progress.
    if (ajax.ajaxing) {
      return false;
    }
 
    try {
      $.ajax(ajax.options);
    }
    catch (err) {
      alert('An error occurred while attempting to process ' + ajax.options.url);
      return false;
    }
 
    return false;
  };
 
  var custom_settings = {};
  custom_settings.url = '/transfer/refresh-elements/ajax/';
  custom_settings.event = 'onload';
  custom_settings.keypress = false;
  custom_settings.prevent = false;
  Drupal.ajax['custom_ajax_action'] = new Drupal.ajax(null, $(document.body), custom_settings);

  $(document).ready(function(){
  
    $('.print-courses.results').hide();
  
    $('body').delegate('.add_to_basket','click',function(){
      $attrs = {};
      $attrs.inst = $(this).attr('inst');
      $attrs.tid = $(this).attr('tid');
      $attrs.tcn = $(this).attr('tcn');
      $attrs.mid = $(this).attr('mid');
      $attrs.mcn = $(this).attr('mcn');
      $attrs.key = $(this).attr('key');     
      
      $(this).html('Added').removeClass('add_to_basket');
      $(this).closest('tr').css('background','#fff');
      
      var html = print_transfer_courses($attrs);
      
      $('#history-box').html(html);
      
      //Drupal.ajax['custom_ajax_action'].specifiedResponse();
    });
    
    $('body').delegate('.remove_from_basket','click',function(){
      $key = $(this).closest('tr').attr('key');
      $('#results-box table tbody a[key="'+$key+'"]').html('Add to basket').addClass('add_to_basket').closest('tr').css('background','#eee');
      $(this).closest('tr').remove();
      
      check_transfer_courses();

    });
    
    $('body').delegate('.print-courses','click',function(){
      PrintElem('#'+$(this).attr('elem'),$(this).attr('path'),$(this).attr('args'));
      $(this).attr('style','');
    });
    
    
  });
  
  function check_transfer_courses(){
    $i = 0;
    $('#courses-list tbody tr').each(function(){
      $i++;
    });
    if($i==0){
      $('.no-courses').show();
      $('.print-courses.results').hide();
    }
  }
  
  function print_transfer_courses(obj){
  
    var html = "<h3>Saved Courses</h3>";
  
    var courses = get_transfer_courses();
    
    $("#courses-list").append(courses);
    
    var newCourse = make_history_obj(obj);
    
    if(courses.indexOf('No Saved Courses') == -1){
      $("#courses-list").append(newCourse);
      $('.no-courses').hide();
      $('.print-courses.results').show();
    } else {
      $path = Drupal.settings.transfer_tool.path;
      $("#history-box").html('<a name="saved"></a><h3>Saved Courses</h3><table id="courses-list"><thead><tr><th>Institution</th><th>Transfer Course ID</th><th>Transfer Course Name</th><th>MMU Course ID</th><th>MMU Course Name</th><th class="optional">Remove</th></tr><tr class="no-courses odd"><td>No Saved Courses</td><td></td><td></td><td></td><td></td><td></td></tr></table><p><a href="javascript:;" class="print-courses" elem="history-box" path="'+$path+'">Print matches</a></p>');
    }
    
  }
  
  function get_transfer_courses(){
    
    var courses = '';
    
    tmp = [];
    
    var html = $('#history-box').text();
      
    if(html.indexOf('No Saved Courses') == -1){
      i = 0;
      $('#history-box .course').each(function(){
        tmp.push($(this).html());
        console.log($(this).html());
        i++;
      });
    } else {
      $('.no-courses').show();
    }
    
    var len = tmp.length;
    var txt = '';
    
    for(i=0;i<len;i++){
      txt += tmp[i];
    }
    
    return txt;
  }
  
  function make_history_obj(obj){
    var markup = '';
    markup += '<tr class="course odd" key="'+obj.key+'">';
      markup += '<td>'+obj.inst+'</td>';
      markup += '<td>'+obj.tid+'</td>';
      markup += '<td>'+obj.tcn+'</td>';
      markup += '<td>'+obj.mid+'</td>';
      markup += '<td>'+obj.mcn+'</td>';
      markup += '<td class="optional"><a href="javascript:void(0)" class="remove_from_basket">Remove</a></td>';
    markup += '</tr>';
    
    return markup;
  }
  
  function PrintElem(elem, path, args){
      Popup($(elem).html(),path, args);
  }

  function Popup(data, path, args){
  
    if(args != ""){
      var heading = "<h1>"+args+" Transer Courses</h1>";
    } else {
      var heading = "";
    }
    
    var mywindow = window.open('', 'Print Transfer Courses', 'height=600,width=800');
    mywindow.document.write('<html><head><title>Print Transfer Courses</title>');
    mywindow.document.write('<link rel="stylesheet" href="'+path+'/css/transfer_tool_print.css" type="text/css" />');
    mywindow.document.write('</head><body >');
    mywindow.document.write('<div id="header"><div id="logo"></div></div><div id="content">');
    mywindow.document.write(heading);
    mywindow.document.write(data);
    mywindow.document.write('</div></body></html>');

    mywindow.print();
    //mywindow.close();

    return true;
  }  
  
  
}(jQuery))