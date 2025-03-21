// This file is for creating a scaffold around a game container
// that is exactly 640 x 480, including controls, and automatic
// resizing of the iframe to fit the scaffold.

var TARGET_WIDTH = 640;
var TARGET_HEIGHT = 480;

const buttonCallbackStrings = {
  Up: {
    down: 'Lib.handleButtonDown(Lib.BUTTON_UP)',
    up: 'Lib.handleButtonUp(Lib.BUTTON_UP)',
  },
  Down: {
    down: 'Lib.handleButtonDown(Lib.BUTTON_DOWN)',
    up: 'Lib.handleButtonUp(Lib.BUTTON_DOWN)',
  },
  Left: {
    down: 'Lib.handleButtonDown(Lib.BUTTON_LEFT)',
    up: 'Lib.handleButtonUp(Lib.BUTTON_LEFT)',
  },
  Right: {
    down: 'Lib.handleButtonDown(Lib.BUTTON_RIGHT)',
    up: 'Lib.handleButtonUp(Lib.BUTTON_RIGHT)',
  },
  Confirm: {
    down: 'Lib.handleButtonDown(Lib.BUTTON_SPACE)',
    up: 'Lib.handleButtonUp(Lib.BUTTON_SPACE)',
  },
  Cancel: {
    down: 'Lib.handleButtonDown(Lib.BUTTON_Z)',
    up: 'Lib.handleButtonUp(Lib.BUTTON_Z)',
  },
  Shift: {
    down: 'Lib.handleButtonDown(Lib.BUTTON_ASSIST)',
    up: 'Lib.handleButtonUp(Lib.BUTTON_ASSIST)',
  },
};

const controlsDpadNormal = `
<div class="scaffold-dpad">
  <div class="scaffold-flex-row-center">
    <button class="scaffold-button scaffold-button-direction"
      onmousedown="${buttonCallbackStrings.Up.down}"
      ontouchstart="${buttonCallbackStrings.Up.down}"
      onmouseup="${buttonCallbackStrings.Up.up}"
      ontouchend="${buttonCallbackStrings.Up.up}"
      >↑</button>
  </div>
  <div class="scaffold-flex-row-space-between">
    <button class="scaffold-button scaffold-button-direction"
      onmousedown="${buttonCallbackStrings.Left.down}"
      ontouchstart="${buttonCallbackStrings.Left.down}"
      onmouseup="${buttonCallbackStrings.Left.up}"
      ontouchend="${buttonCallbackStrings.Left.up}"
      >←</button>
    <button class="scaffold-button scaffold-button-direction"
      onmousedown="${buttonCallbackStrings.Right.down}"
      ontouchstart="${buttonCallbackStrings.Right.down}"
      onmouseup="${buttonCallbackStrings.Right.up}"
      ontouchend="${buttonCallbackStrings.Right.up}"
      >→</button>  
  </div>
  <div class="scaffold-flex-row-center">
    <button class="scaffold-button scaffold-button-direction"
      onmousedown="${buttonCallbackStrings.Down.down}"
      ontouchstart="${buttonCallbackStrings.Down.down}"
      onmouseup="${buttonCallbackStrings.Down.up}"
      ontouchend="${buttonCallbackStrings.Down.up}"
      >↓</button>
  </div>
</div>
`;

const controlsDpadLr = `
<div class="scaffold-dpad">
  <div class="scaffold-flex-row-space-between">
    <button class="scaffold-button scaffold-button-direction"
      onmousedown="${buttonCallbackStrings.Left.down}"
      ontouchstart="${buttonCallbackStrings.Left.down}"
      onmouseup="${buttonCallbackStrings.Left.up}"
      ontouchend="${buttonCallbackStrings.Left.up}"
      >←</button>
    <button class="scaffold-button scaffold-button-direction"
      onmousedown="${buttonCallbackStrings.Right.down}"
      ontouchstart="${buttonCallbackStrings.Right.down}"
      onmouseup="${buttonCallbackStrings.Right.up}"
      ontouchend="${buttonCallbackStrings.Right.up}"
      >→</button>  
  </div>
</div>
`;

const controlsButtonsAb = `
<div class="scaffold-buttons">
  <div class="scaffold-flex-row-flex-end">
    <button class="scaffold-button scaffold-button-confirm"
      onmousedown="${buttonCallbackStrings.Confirm.down}"
      ontouchstart="${buttonCallbackStrings.Confirm.down}"
      onmouseup="${buttonCallbackStrings.Confirm.up}"
      ontouchend="${buttonCallbackStrings.Confirm.up}"
      >A</button>
  </div>
  <div class="scaffold-flex-row-center">
    <button class="scaffold-button scaffold-button-cancel"
      onmousedown="${buttonCallbackStrings.Cancel.down}"
      ontouchstart="${buttonCallbackStrings.Cancel.down}"
      onmouseup="${buttonCallbackStrings.Cancel.up}"
      ontouchend="${buttonCallbackStrings.Cancel.up}"
      >B</button>
  </div>
</div>
`;

const controlsButtonsAbShift = `
<div class="scaffold-buttons">
  <div class="scaffold-flex-row-center">

  </div>
  <div class="scaffold-flex-row-space-between">
    <button class="scaffold-button scaffold-button-shift"
      onmousedown="${buttonCallbackStrings.Shift.down}"
      ontouchstart="${buttonCallbackStrings.Shift.down}"
      onmouseup="${buttonCallbackStrings.Shift.up}"
      ontouchend="${buttonCallbackStrings.Shift.up}"
      >X</button>
    <button class="scaffold-button scaffold-button-confirm"
      onmousedown="${buttonCallbackStrings.Confirm.down}"
      ontouchstart="${buttonCallbackStrings.Confirm.down}"
      onmouseup="${buttonCallbackStrings.Confirm.up}"
      ontouchend="${buttonCallbackStrings.Confirm.up}"
      >A</button>
  </div>
  <div class="scaffold-flex-row-center">
    <button class="scaffold-button scaffold-button-cancel"
      onmousedown="${buttonCallbackStrings.Cancel.down}"
      ontouchstart="${buttonCallbackStrings.Cancel.down}"
      onmouseup="${buttonCallbackStrings.Cancel.up}"
      ontouchend="${buttonCallbackStrings.Cancel.up}"
      >B</button>
  </div>
</div>
`;

let scaffoldOrientation = 'portrait';
let controlsVisible = true;
const mediaQuery = window.matchMedia('(orientation: portrait)');
function handleScreenSizeChange(e) {
  if (e.matches) {
    // Media query matches
    scaffoldOrientation = 'portrait';
  } else {
    // Media query doesn't match
    scaffoldOrientation = 'landscape';
  }
  if (!controlsVisible) {
    scaffoldHideControls();
  } else {
    scaffoldShowControls();
  }
}
handleScreenSizeChange(mediaQuery);
mediaQuery.addEventListener('change', handleScreenSizeChange);

// dpadLayout: normal, lr
// buttonsLayout: 'ab', 'abshift'
function scaffoldSetupBoard(dpadLayout, buttonsLayout) {
  const controlsDpad =
    dpadLayout === 'lr' ? controlsDpadLr : controlsDpadNormal;
  const controlsButtons =
    buttonsLayout === 'abshift' ? controlsButtonsAbShift : controlsButtonsAb;

  const scaffoldHtml = `
  <div class="scaffold-outer">
    <div id="top-bar-controls" class="top-bar-controls"></div>
    <div class="scaffold-inner">
      <div id="scaffold-controls-left" class="scaffold-controls">
        ${controlsDpad}
        <div class="scaffold-vertical-spacer"></div>
      </div>
      <div id="board" class="board scaffold-board">
        <canvas id="canvas" width="${TARGET_WIDTH}" height="${TARGET_HEIGHT}"></canvas>
      </div>
      <div id="scaffold-controls-right" class="scaffold-controls">
        ${controlsButtons}
        <div class="scaffold-vertical-spacer"></div>
      </div>
      <div id="scaffold-controls-bottom">
        <div class="scaffold-controls">
          ${controlsDpad}
          <div class="scaffold-vertical-spacer"></div>
        </div>
        <div class="scaffold-controls">${controlsButtons}
        <div class="scaffold-vertical-spacer"></div>
        </div>
      </div>
    </div>
  </div>
  `;
  const game = document.getElementById('game');
  if (game) {
    game.innerHTML = scaffoldHtml;
  } else {
    console.error('Could not find "#game" element to append scaffold.');
  }
}

function scaffoldHideControls() {
  controlsVisible = false;

  const controlsLeft = document.getElementById('scaffold-controls-left');
  const controlsRight = document.getElementById('scaffold-controls-right');
  const controlsBottom = document.getElementById('scaffold-controls-bottom');
  const board = document.getElementById('board');
  const canvas = document.getElementById('canvas');
  /** @type {any} */
  const scaffoldInner = document.querySelector('.scaffold-inner');

  if (controlsLeft) {
    controlsLeft.style.display = 'none';
    controlsLeft.style.visibility = 'hidden';
  }
  if (controlsRight) {
    controlsRight.style.display = 'none';
    controlsRight.style.visibility = 'hidden';
  }
  if (controlsBottom) {
    controlsBottom.style.visibility = 'hidden';
    if (Lib.getConfig().isFullScreen) {
      controlsBottom.style.setProperty('display', 'none', 'important');
    }
  }
  if (board) {
    board.style.width = '100%';
  }
  if (canvas) {
    if (scaffoldOrientation === 'landscape') {
      canvas.style.width = 'auto';
      canvas.style.maxWidth = 'unset';
      if (Lib.getConfig().isFullScreen) {
        canvas.style.height = '100%';
      } else {
        canvas.style.height = 'calc(100% - 42 * 2px)';
      }
    }
  }
  if (scaffoldInner) {
    scaffoldInner.style.height = '100%';
  }
}

function scaffoldShowControls() {
  controlsVisible = true;
  const controlsLeft = document.getElementById('scaffold-controls-left');
  const controlsRight = document.getElementById('scaffold-controls-right');
  const controlsBottom = document.getElementById('scaffold-controls-bottom');
  const board = document.getElementById('board');
  const canvas = document.getElementById('canvas');
  /** @type {any} */
  const scaffoldInner = document.querySelector('.scaffold-inner');

  if (controlsLeft) {
    controlsLeft.style.display = '';
    controlsLeft.style.visibility = '';
  }
  if (controlsRight) {
    controlsRight.style.display = '';
    controlsRight.style.visibility = '';
  }
  if (controlsBottom) {
    controlsBottom.style.visibility = '';
    if (Lib.getConfig().isFullScreen) {
      controlsBottom.style.display = '';
    }
  }
  if (board) {
    board.style.width = '';
  }
  if (canvas) {
    canvas.style.height = '';
    canvas.style.width = '';
    canvas.style.maxWidth = '';
  }
  if (scaffoldInner) {
    scaffoldInner.style.height = '';
    scaffoldInner.style.transform = '';
  }
}
