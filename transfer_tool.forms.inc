<?php

function transfer_tool_form($form, &$form_state, $vars){
  $form = array();
  
  //$course = (!empty($vals['courseId'])) ? $vals['courseId'] : "";
  
  $form['OrgId'] = array(
    '#title' => 'Institution',
    '#type' => 'select',
    '#options' => $vars,
    '#default_value' => $vals['orgId'],
  );
  
  $form['CourseId'] = array(
    '#title' => 'Course',
    '#type' => 'textfield',
    '#size' => 30,
    '#description' => 'Enter a course ID from your institution to view its Mount Mercy equivalent (Ex: CS 101) or <a href="#saved">Jump to basket</a>',
    '#default_value' => $course,
  );
  
  $form['submit'] = array(
	  '#type' => 'submit',
	  '#value' => t('Search for a course'),
	  '#submit' => array('transfer_tool_form_submit'),
	  '#ajax' => array(
      'callback' => 'transfer_tool_form_submit',
      'wrapper' => 'results-box',
    ),
	);
	
  $form['box'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="results-box">',
    '#suffix' => '</div>',
    '#markup' => '<h3>Search for a course, or leave blank for all eligible courses from school</h3>',
  );
  
  $form['history-box'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="history-box">',
    '#suffix' => '</div>',
    '#markup' => '<a name="saved"></a><h3>My Courses</h3><table id="courses-list"><thead><tr><th>Transfer Course ID</th><th>Transfer Course Name</th><th>MMU Course ID</th><th>MMU Course Name</th><th class="optional">Remove</th></tr><tr class="no-courses odd"><td>No Saved Courses</td><td></td><td></td><td></td><td></td></tr></table><p><a href="javascript:;" class="print-courses results" elem="history-box" path="'.drupal_get_path('module','transfer_tool').'">Print courses</a></p>',
  );
  
  return $form;
}

function transfer_tool_form_callback($form,&$form_state){
  return $_SESSION['transfer_tool_history']['tmp'];
}

function transfer_tool_form_submit($form,&$form_state){

  $_SESSION['transfer_tool_history']['tmp'] = '';
  
  $values = $form_state['values'];
  
  $search_org_id = $values['OrgId'];
  $search_course_id = str_replace(' ', '', trim($values['CourseId'])) . '%';
  
  $vals = array(
    'orgId' => $search_org_id,
    'courseId' => $values['CourseId'],
  );
  
  //$_SESSION['transfer_history_results'] = $vals;
	
	// Do search for courses
	$transferCoursesResult = false;
	if(!empty($search_org_id) && !empty($search_course_id)) {
	 
		$transferCoursesResult = db_query("SELECT ORG_CODE_ID, TRANSFER_EVENT_ID, TRANSFER_EVENT_TYPE, TRANSFER_COURSE_NAME, MMU_EVENT_ID, MMU_EVENT_TYPE, MMU_COURSE_NAME FROM {transfer_tool_courses} WHERE ORG_CODE_ID = :org_code_id AND REPLACE(TRANSFER_EVENT_ID,' ','') LIKE :transfer_event", array(":org_code_id" => $search_org_id, ":transfer_event" => $search_course_id));

	} else {
	
		$transferCoursesResult = db_query("SELECT ORG_CODE_ID, TRANSFER_EVENT_ID, TRANSFER_EVENT_TYPE, TRANSFER_COURSE_NAME, MMU_EVENT_ID, MMU_EVENT_TYPE, MMU_COURSE_NAME FROM {transfer_tool_courses} WHERE ORG_CODE_ID = :org_code_id", array(":org_code_id" => $search_org_id)); 
		
	}
	
	if ($transferCoursesResult->rowCount() == 0)  { 
		drupal_set_message("Your search didn't return any results. Please try again"); 
	} else {

	  $header = array(
      array(
        'data' => t('Transfer Course ID'),
      ),
      array(
        'data' => t('Transfer Course Name'),
      ),
      array(
        'data' => t('MMU Course ID'),    
      ),
      array(
        'data' => t('MMU Course Name'),    
      ),
      array(
        'data' => t('Add to Basket'), 
        'class' => 'optional',   
      ),
	  );
    $i = 0;
/*
		foreach($transferCoursesResult as $course){
		  $key['key'] = $course->ORG_CODE_ID.'+'.trim(str_replace(' ','',$course->TRANSFER_EVENT_ID));
		  $key['count'] = $i;
		  $form = drupal_get_form('transfer_tool_set_history_form_'.$i,$key);
		  //dpm($form);
		  //dpm(drupal_get_form('transfer_tool_set_history_form',$key));
		  $output[] = array(
		    $course->TRANSFER_EVENT_ID,
		    $course->TRANSFER_COURSE_NAME,
		    $course->MMU_EVENT_ID,
		    $course->MMU_COURSE_NAME,
		    drupal_render($form),
	    );
	   $i++;
		}
*/
    $output = array();
  	foreach($transferCoursesResult as $course){
  	  $key['key'] = $course->ORG_CODE_ID.'+'.trim(str_replace(' ','',$course->TRANSFER_EVENT_ID));
  	  $key['count'] = $i;
  	  //dpm($form);
  	  //dpm(drupal_get_form('transfer_tool_set_history_form',$key));
  	  $output[] = array(
  	    'tid' => $course->TRANSFER_EVENT_ID,
  	    'tcn' => $course->TRANSFER_COURSE_NAME,
  	    'mid' => $course->MMU_EVENT_ID,
  	    'mcn' => $course->MMU_COURSE_NAME,
  	    'key' => array('data' => '<a href="javascript:void(0)" class="add_to_basket btn optional" tid="'.$course->TRANSFER_EVENT_ID.'" tcn="'.$course->TRANSFER_COURSE_NAME.'" mid="'.$course->MMU_EVENT_ID.'" mcn="'.$course->MMU_COURSE_NAME.'" key="'.$key['key'].'">Add to basket</a>','class' => array('optional')),
      );
     $i++;
  	}
  }	
	
	$form_state['rebuild'] = TRUE;

  $vars['header'] = $header;
  $vars['rows'] = $output;
  $vars['attributes'] = array();
  
  //$form = drupal_get_form('transfer_tool_set_history_form',$vars);
  
  //$markup .= drupal_render($form);
  
  $table = theme_table($vars);
  
  $table .= '<p><a href="javascript:;" class="print-courses" elem="results-box" path="'.drupal_get_path('module','transfer_tool').'">Print matches</a></p>';
	
  $element = $form['box'];
  $element['#markup'] = $table;
  return $element; 

}

function transfer_tool_refresh($action, $type) {
  if ($type != 'ajax') {
    // This is js only.
    return 'Oh well';
  }
 
  $commands = array();
  $commands[] = ajax_command_replace('#history-box', date('G:i:s'));
  $page = array('#type' => 'ajax', '#commands' => $commands);
  ajax_deliver($page);
}

/**
  * History Add Form
  */
  
function transfer_tool_set_history_form($form, &$form_state, $vars){
	
	// $_SESSION['History'] stores Saved Search results.
	if (empty($_SESSION['transfer_history_history'])) {
		$_SESSION['transfer_history_tmp'] = array();
	}  

  $options = array();
  foreach($vars['rows'] as $row){
    $options[$row['key']] = array(
      'tid' => $row['tid'],
      'tcn' => $row['tcn'],
      'mid' => $row['mid'],
      'mcn' => $row['mcn'],
    );
  }
  
  $header = array();
  foreach($vars['header'] as $row){
    foreach($row as $key => $value){
      $header[$key] = $value;
    }
  }
  	
	$form = array();
	
	$form['table'] = array(
	 '#type' => 'tableselect',
	 '#header' => $header,
	 '#options' => $options,
	);
	 
	$form['submit'] = array(
	 '#type' => 'submit',
	 '#value' => 'Save Selected Courses',
	 '#name' => 'save_courses',
	 '#submit' => array('transfer_tool_history_form_submit'),
	 '#limit_validation_errors' => array(),
	  '#ajax' => array(
      'callback' => 'transfer_tool_history_form_callback',
      'wrapper' => 'history-box',
      'effect' => 'fade',
    ),
    //'#id' => 'historysubmit',
	); 
	
  $form['history-box'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="history-box">',
    '#suffix' => '</div>',
    '#markup' => '<h2>Saved Courses</h2><p>No courses saved yet</p>',
  );
	
	if(!empty($form_state['history-box'])) {
   $form['history-box']['history']['#theme'] = 'history_box_theme';
   foreach($form_state['history-box'] as $key => $option) {
      $form['history-box']['history'][$key] = history_box_form_el($option);
    }
  }	
	
	return $form;
	
}

function transfer_tool_history_form_callback($form, &$form_state){
  return transfer_tool_session();
}

function transfer_tool_history_form_submit($form, &$form_state){
  
  if(!empty($form_state['values']['save_courses'])){
    $weight = (!empty($form_state['history-box'])) ? sizeof($form_state['history-box']) : 0;
    $form_state['history-box'][] = array(
      'remove' => 0,
      'title' => '',
      'weight' => $weight,
      'option-id' => 0,
    );
  }
  $form_state['rebuild'] = TRUE;  

  $rows = array();
  
  foreach($form_state['values']['table'] as $key => $row){
    if(!empty($row)){
      
      $vals = $row;
      $vals = str_replace("+", ",", $vals);
      $vals = explode(",", $vals);
      
      $result = db_query("SELECT ORG_CODE_ID, TRANSFER_EVENT_ID, TRANSFER_EVENT_TYPE, TRANSFER_COURSE_NAME, MMU_EVENT_ID, MMU_EVENT_TYPE, MMU_COURSE_NAME FROM {transfer_tool_courses} WHERE ORG_CODE_ID = :org_code_id AND REPLACE(TRANSFER_EVENT_ID,' ','') = :transfer_event", array(":org_code_id" => $vals[0], ":transfer_event" => $vals[1]));
      
      $record = $result->fetchObject();
      
      $rows[] = array(
        'tid' => $record->TRANSFER_EVENT_ID,
        'tcn' => $record->TRANSFER_COURSE_NAME,
        'mid' => $record->MMU_EVENT_ID,
        'mcn' => $record->MMU_COURSE_NAME,
        'key' => $record->ORG_CODE_ID . "+" . $record->TRANSFER_EVENT_ID,
      );
    }
  }
  
  $header = array(
    'tid' => t('Transfer Course ID'),
    'tcn' => t('Transfer Course Name'),
    'mid' => t('MMU Course ID'),    
    'mcn' => t('MMU Course Name'),    
  );
  
  if(!empty($_SESSION['transfer_tool_history']['rows'])){
    $rows = array_merge($_SESSION['transfer_tool_history']['rows'],$rows);
  }
  
  $vars['header'] = $header;
  $vars['rows'] = $rows;
  
  $_SESSION['transfer_tool_history'] = $vars;
  
  $markup = "<h2>Saved Courses</h2>";
  
  $markup .= drupal_render(drupal_get_form('transfer_tool_remove_history_form',$vars));

  $element = $form['box'];
  $element['#markup'] = $markup;
  return $element; 

}

function transfer_tool_session(){
  $rows = array();
  foreach($_SESSION['transfer_tool_history']['rows'] as $course){
    $rows[] = array(
      'tid' => $course['tid'],
      'tcn' => $course['tcn'],
      'mid' => $course['mid'],
      'mcn' => $course['mcn'],
      
    );
  }

  $vars['header'] = $_SESSION['transfer_tool_history']['header'];
  $vars['rows'] = $rows;
  $vars['attributes'] = array();

  
  $table = theme_table($vars);
  
  return $table;
}

function history_box_form_el($option = array()) {
   $element = array(
      'remove' => array(
         '#type' => 'checkbox',
         '#title' => '',
         '#default_value' => (!empty($option['remove'])) ? $option['remove'] : '',
         '#attributes' => array('class' => array('remove')),
      ),
      'title' => array(
         '#type' => 'textfield',
         '#title' => '',
         '#default_value' => (!empty($option['title'])) ? $option['title'] : '',
         '#attributes' => array('class' => array('title')),
      ),
      'weight' => array(
         '#type' => 'hidden',
         '#default_value' => (!empty($option['weight'])) ? $option['weight'] : '',
      ),
      'option_id' => array(
         '#type' => 'hidden',
         '#default_value' => (!empty($option['option_id'])) ? $option['option_id'] : '',
      ),
   );
   return $element;
}

/**
  * History Remove Form
  */
  
function transfer_tool_remove_history_form($form, &$form_state, $vars){
	
  $options = array();
  foreach($vars['rows'] as $row){
    $options[$row['key']] = array(
      'tid' => $row['tid'],
      'tcn' => $row['tcn'],
      'mid' => $row['mid'],
      'mcn' => $row['mcn'],
    );
  }
  
/*
  $header = array();
  foreach($vars['header'] as $row){
    foreach($row as $key => $value){
      $header[$key] = $value;
    }
  }
*/
  	
	$form = array();
	
	$form['table'] = array(
	 '#type' => 'tableselect',
	 '#header' => $vars['header'],
	 '#options' => $options,
	);
	
/*
  $form['saved-box'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="history-box">',
    '#suffix' => '</div>',
    '#markup' => '<h2>Saved Courses</h2><p>No courses saved yet</p>',
  );
*/
	 
	$form['submit'] = array(
	 '#type' => 'submit',
	 '#value' => 'Remove Courses',
	  '#ajax' => array(
      'callback' => 'transfer_tool_history_form_submit',
      'wrapper' => 'history-box',
      'effect' => 'fade',
    ),
	); 
	
	return $form;
	
}

/*
function transfer_tool_remove_history_form_submit($form, &$form_state){

  $markup = 'test';

  $element = $form['history-box'];
  $element['#markup'] = $markup;
  return $element; 
  
}
*/