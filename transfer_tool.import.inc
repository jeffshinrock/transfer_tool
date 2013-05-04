<?php
/*
 * import.php 
 * Created by Derek Hartwig and Eric Watkins for Mount Mercy University
 * 
 * In association with: John Edwards, Michelle Carter, and Herman Hounkponou.
 * Parses the .csv file and reads it into the database for the Transfer Policy site.
*/

function transfer_tool_import_data($type, $url){

  //ini_set('max_execution_time', 300);
  //ini_set('memory_limit', '1024M');
  
  $rows = array();
  $rowcnt = 0;
  $errors = array();
  
  $fin = fopen($url,'r');

  switch($type){
    
    case 'orgs':
    
      db_truncate('transfer_tool_orgs')->execute();
      
      while (($data=fgetcsv($fin,5000,","))!==FALSE) {
      
        if($rowcnt >= 1){
          $record = array(
            'org_code_id' => $data[0],
            'org_name' => $data[1],
          );
          array_push($rows, $record);
        }
        $rowcnt++;
      }
      
      $db = 'transfer_tool_orgs';
      
      break;
    
    case 'courses': 
    
      db_truncate('transfer_tool_courses')->execute();
      
      while (($data=fgetcsv($fin,5000,","))!==FALSE) {
      
        if($rowcnt >= 1){
          $record = array(
            'org_code_id' => $data[0],
            'org_name' => $data[1],
            'transfer_event_id' => $data[2],
            'transfer_event_type' => $data[3],
            'transfer_course_name' => $data[4],
            'mmu_event_id' => $data[5],
            'mmu_event_type' => $data[6],
            'mmu_course_name' => $data[7],
          );
          array_push($rows, $record);
        }
        $rowcnt++;
      }
      
      $db = 'transfer_tool_courses';
      
      break;
  }
  
  fclose($fin);
  
  foreach($rows as $row){
    drupal_write_record($db,$row);
  }
  
  drupal_set_message('transfer ' . $type . ' updated');

  //if(isset($_GET["pass"]) && $_GET["pass"] == '8rafepRa') {
  
  
//old server connection deets
/*
  	// Connect to the Transfer Policy Database (EDIT THIS)
  	$server = "173.203.60.79";
  	$username = "transfer";
  	$password = "M0untM3rcy";
  	$connect = mysql_connect($server, $username, $password);
  
  	// Make sure that it connected successfully.
  	if(!$connect) {
  		die('Could not connect: ' . mysql_error());
  	}
  	
  	// Select the Transfer Policy Table.
  	mysql_selectdb("mydomains_transfer", $connect);
*/
  
  	// Parse the CSV Files
  	//$file = "transfer-policy.csv";
  	
/*
  	if(file_exists($url)) {
  	
  		// Clear the Old Data.
  		db_truncate('transfer_tool_courses')->execute();
  		db_truncate('transfer_tool_orgs')->execute();
  
  		$query = "LOAD DATA LOCAL INFILE '" . $file . "' INTO {transfer_tool_orgs} FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\\r\\n' IGNORE 1 LINES (ORG_CODE_ID, ORG_NAME, @temp1, @temp2, @temp3, @temp4, @temp5, @temp6);";
  
  		db_query($query);
  
  		$query = "LOAD DATA LOCAL INFILE '" . $file . "' INTO {transfer_tool_courses} FIELDS TERMINATED BY ',' OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\\r\\n' IGNORE 1 LINES (ORG_CODE_ID, @temp1, TRANSFER_EVENT_ID, TRANSFER_EVENT_TYPE, TRANSFER_COURSE_NAME, MMU_EVENT_ID, MMU_EVENT_TYPE, MMU_COURSE_NAME);";
  	
  		db_query($query);
  		
  		drupal_set_message('Transfer course table updated');
  	
  	} else {
  		drupal_set_message('File could not be found at ' . $url . '. Upload failed.','error');
  	}
*/
  
  	//closes the database
  	//mysql_close ($connect);
  //}

}

function csv_to_array($filename='', $delimiter=','){
  
  if(!file_exists($filename) || !is_readable($filename)){
    drupal_set_message('Error loading file','error');
  }

  $header = NULL;
  $data = array();
  if (($handle = fopen($filename, 'r')) !== FALSE){
    while (($row = fgetcsv($handle, 1000, $delimiter, '"')) !== FALSE){
      if(!$header){
        $header = $row;
      }
      else{
        $data[] = array_combine($header, $row);
      }
    }
    fclose($handle);
  }
  return $data;
}
