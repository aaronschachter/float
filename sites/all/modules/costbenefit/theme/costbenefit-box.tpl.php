<?php
/*
@todo: Add total counts.  Also breakdown by short term long term.
*/
?>
<div>
  <table class="table table-striped table-bordered">
    <thead>
      <tr>
      <th colspan="2">
        <?php print _costbenefit_get_box_type($vars['box']) ?>
        <span class="badge"><?php print count($vars['items']); ?></span>
        <button class="btn btn-tertiary pull-right" data-toggle="modal" data-target=".add-item-modal-<?php print $vars['box']; ?>">+</button>
      </tr>
    </thead>
    <tbody>
    <?php foreach ($vars['items'] as $item): ?>
      <tr>
        <td><?php print l($item->title, 'cb/' . $vars['cb_id'] . '/' . $item->cb_item_id); ?></td>
        <td><?php print $item->type_desc; ?></td>
      </tr>
    <?php endforeach; ?>
    </tbody>
  </table>
  <div class="modal fade add-item-modal-<?php print $vars['box']; ?>" tabindex="-1" role="dialog" aria-labelledby="myLargeModalLabel" aria-hidden="true">
    <div class="modal-dialog modal-lg">
      <div class="modal-content">
        <div class="modal-header">
          <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
          <h4 class="modal-title">Add</h4>
        </div>
        <div class="modal-body">
          <p><?php print render($add_form); ?></p>
        </div>
      </div>
    </div>
  </div>
</div>
