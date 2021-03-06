<?php

/**
 * @file
 * Provides classes for the Costbenefit Entity.
 */

/**
 * Our Costbenefit entity class.
 */
class CostbenefitEntity extends Entity {
  protected $item_table = 'costbenefit_item';

  /**
   * Override construct to add box items.
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, $entityType);
    if (isset($this->cb_id)) {
      for ($i= 1; $i <= 4; $i++) {
        $this->boxes[$i] = $this->getBoxItems($i, FALSE);
      }
    }
  }

  /**
   * Override this in order to implement a custom default URI.
   */
  protected function defaultUri() {
    return array('path' => 'cb/' . $this->identifier());
  }

  /**
   * Override this in order to implement a custom default URI.
   */
  public function getUid() {
    return $this->uid;
  }

  /**
   * Returns array of costbenefit_item objects for entity's given $box.
   */
  public function getBoxItems($box, $include_form = TRUE) {
    $items = array();
    $query = db_select($this->item_table, 'cbi')
      ->fields('cbi')
      ->condition('cb_id', $this->cb_id)
      ->condition('box', $box);
    $result = $query->execute()->fetchAll();

    foreach ($result as $delta => $record) {
      $item_entity = costbenefit_item_load($record->cb_item_id);
      $items[$delta] = new StdClass();
      $items[$delta]->cb_item_id = $item_entity->cb_item_id;
      $items[$delta]->title = $item_entity->title;
      $items[$delta]->type_desc = $record->type ? 'Long term' : 'Short term';
      if ($include_form) {
        $items[$delta]->form = drupal_get_form('walter_costbenefit_item_edit_form', $item_entity);
      }
    }

    return $items;
  }
}

/**
 * Our custom controller for the Costbenefit entity.
 */
class CostbenefitEntityController extends EntityAPIController {

  /**
   * Overrides buildContent() method.
   *
   * Adds Costbenefit properties and items into the content array.
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    module_load_include('inc', 'walter_costbenefit', 'walter_costbenefit.forms');
    $build = parent::buildContent($entity, $view_mode, $langcode, $content);
    $build['title'] = check_plain($entity->title);
    $build['edit_form'] = drupal_get_form('walter_costbenefit_form', $entity);
    $boxes = array();
    // Loop through the boxes.
    for ($i = 1; $i <= 4; $i++) {
      $link = 'cb/' . $entity->cb_id . '/add/' . $i;
      $vars = array(
        'title' => check_plain($entity->title),
        'cb_id' => $entity->cb_id,
        'box' => $i,
        'items' => $entity->getBoxItems($i),
        'link' => $link,
      );
      $add_form = drupal_get_form('walter_costbenefit_item_add_form_' . $i, $entity, $i);
      $build['boxes'][$i] = array(
        '#markup' => theme('costbenefit_box', array(
          'vars' => $vars,
          'add_form' => $add_form,
        )),
      );
    }
    return $build;
  }

  /**
   * Overrides save() method.
   *
   * Populates created and uid automatically.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    global $user;
    $now = REQUEST_TIME;
    if (isset($entity->is_new)) {
      $entity->created = $now;
      $entity->uid = $user->uid;
    }
    $entity->updated = $now;
    parent::save($entity, $transaction);
  }
}


/**
 * Our Costbenefit Item entity class.
 */
class CostbenefitItemEntity extends Entity {

  /**
   * Override construct to add the parent CB uid.
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, $entityType);
  }

  /**
   * Override this in order to implement a custom default URI.
   */
  protected function defaultUri() {
    return array('path' => 'cb-item/' . $this->identifier());
  }

  public function getUid() {
    return walter_costbenefit_get_cb_uid($this->cb_id);
  }
}

/**
 * Our custom controller for the Costbenefit entity.
 */
class CostbenefitItemEntityController extends EntityAPIController {

  /**
   * Overrides save() method.
   *
   * Sets parent costbenefit updated property.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    global $user;
    if ($user->uid != 1) {
      // Load the parent costbenefit.
      $costbenefit = costbenefit_load($entity->cb_id);
      // If current user is not the entity author:
      if ($costbenefit->uid != $user->uid) {
        // HTML injection attack.  Set error message and don't save.
        drupal_set_message(t('Sorry, an error as occurred.'), 'error');
        return FALSE;
      }
    }
    parent::save($entity, $transaction);
    if ($user->uid != 1) {
      // Save to update the timestamp.
      $costbenefit->save();
    }
  }
}

