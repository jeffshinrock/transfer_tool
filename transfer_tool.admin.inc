<?php

function transfer_tool_settings_form() {
  $form = array();
    
  $form['transfer_tool_page_text'] = array(
    '#title' => 'Page text for transfer tool page',
    '#type' => 'text_format',
    '#text_format' => TRUE,
    '#default_value' => variable_get('transfer_tool_page_text',''),
  );
  
/*
  $form['file_url'] = array(
    '#type' => 'textfield',
    '#title' => 'Recurring upload URL',
    '#description' => 'If this is populated, the system will attempt to get the file and update the database on cron',
  );
*/
  
  $form['csv'] = array(
    '#prefix' => '<hr>',
    '#type' => 'managed_file',
    '#title' => 'Manual Upload',
    '#description' => 'Manually upload a new transfer file (csv only)',
    '#upload_location' => 'public://files/',
    '#upload_validators'  => array(
      'file_validate_extensions' => array("csv"),
      'file_validate_size' => array(1024*1024*800),
      ),
  );
  
  $form['type'] = array(
    '#type' => 'select',
    '#title' => 'Upload Type',
    '#options' => array('0' => '--select--', 'orgs' => 'Organizations', 'courses' => 'Courses'),
    '#description' => 'If you have manually uploading a file, type is required',
  );
  
 $form['submit'] = array(
	  '#type' => 'submit',
	  '#value' => t('Save'),
	);
  
  return $form;
}

function transfer_tool_settings_form_validate($form, &$form_state){
    
  if(!empty($form_state['values']['csv']) && empty($form_state['values']['type'])){
    form_set_error('type','Type must be selected for a file upload');
  }
}

function transfer_tool_settings_form_submit($form, &$form_state){
  
  if(!empty($form_state['values']['csv']) && !empty($form_state['values']['type'])){
    $data = transfer_tool_import_data($form_state['values']['type'],drupal_realpath($form['csv']['#file']->uri));
    dpm($data);
  }
  
  variable_set('transfer_tool_page_text',$form_state['values']['transfer_tool_page_text']['value']);
}

