<?php

/**
 * Implements hook_block_info().
 */
function transfer_tool_block_info() {
  $blocks = array();
  $blocks['transfer_tool'] = array(
    'info' => t('Transfer Tool Block'),
  );
 
  return $blocks;
}

/**
 * Implements hook_block_view().
 */
function transfer_tool_block_view($delta='') {
  $block = array();
 
  switch($delta) {
    case 'transfer_tool' :
      $block['content'] = transfer_tool_view();
      break;
  }
 
  return $block;
}

function transfer_tool_view() {
  $block = array();
  
  $block = array(
    'message' => array(
    '#type' => 'markup',
    '#markup' => render(drupal_get_form('transfer_tool_form',transfer_tool_get_orgs())),
    '#suffix' => '</div>',
    ),
  );
  
  return $block;
  
}