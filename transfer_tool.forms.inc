<?php

function transfer_tool_form($form, &$form_state, $vars){
  $form = array();
  
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

    $output = array();
  	foreach($transferCoursesResult as $course){
  	  $key['key'] = $course->ORG_CODE_ID.'+'.trim(str_replace(' ','',$course->TRANSFER_EVENT_ID));
  	  $key['count'] = $i;
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

