{-# LANGUAGE OverloadedStrings #-}
{-# LANGUAGE ScopedTypeVariables #-}

import Control.Monad
import Control.Monad.ST
import Control.Monad.Trans.State
import Control.Monad.IO.Class
import Data.Text (Text)
import Data.Word
import Foreign.C.Types
import SDL hiding (get)
import qualified SDL.Font as TTF

type Game = StateT GameState IO

data GameState = GameState
  { window :: Window
  , renderer :: Renderer
  , timer :: Word32
  , bitmap :: Surface
  , statestack :: [Game ()]
  }

windowWidth = 800
windowHeight = 600
windowCaption = "SDL Introduction"

fps = 30
frameRate = 1000 / fps

main :: IO ()
main = do
  initializeAll
  window <- createWindow windowCaption defaultWindow
  let dr = defaultRenderer { rendererType = SoftwareRenderer }
  renderer <- createRenderer window (-1) dr
  timer <- ticks
  bitmap <- loadBMP "../../shared/background.bmp"
  runStateT runGame (GameState window renderer timer bitmap [menu])
  return ()

-- Functions without explicit signatures are :: Game ()
runGame :: Game ()
runGame = do
  initGame
  gameLoop
  shutdown

gameLoop = do
  stack <- gets statestack 
  unless (null stack) $ do
    head stack
    gameLoop

initGame = do
  SDL.initialize [InitVideo, InitTimer]
  TTF.initialize

shutdown = do
  b <- gets bitmap
  freeSurface b
  quit
  TTF.quit

menu = do
  fhe <- frameHasElapsed
  when fhe $ do
    handleInput handleMenuInput
    clearScreen
    displayText "(S)tart game" 350 250 20 255 255 255 0 0 0
    displayText "(Q)uit game" 350 275 20 255 255 255 0 0 0
    finishFrame

game = do
  fhe <- frameHasElapsed
  when fhe $ do
    handleInput handleGameInput
    clearScreen
    drawBackground
    finishFrame

exit = do
  fhe <- frameHasElapsed
  when fhe $ do
    handleInput handleExitInput
    clearScreen
    displayText "Quit Game (Y or N)?" 350 250 12 255 255 255 0 0 0
    finishFrame

drawBackground = do
  renderer <- gets renderer
  bitmap <- gets bitmap
  texture <- createTextureFromSurface renderer bitmap
  copy renderer texture Nothing Nothing

displayText :: Text -> CInt -> CInt -> CInt -> Word8 -> Word8 -> Word8 -> 
               Word8 -> Word8 -> Word8 -> Game ()
displayText text x y size_ fR fG fB bR bG bB = do
  renderer <- gets renderer
  font <- TTF.load "../../shared/arial.ttf" 24
  surface <- TTF.shaded font (V4 fR fG fB 1) (V4 bR bG bB 1) text 
  (w', h') <- TTF.size font text
  let (w, h) :: (CInt, CInt) = (fromIntegral w', fromIntegral h')
  texture <- createTextureFromSurface renderer surface
  copy renderer texture Nothing $ Just $ Rectangle (P (V2 x y)) (V2 w h)
  freeSurface surface
  TTF.free font

clearScreen = do
  renderer <- gets renderer
  clear renderer

frameHasElapsed :: Game Bool
frameHasElapsed = do
  t <- gets timer
  t' <- ticks 
  return $ fromIntegral t' - fromIntegral t >= frameRate

finishFrame = do
  window <- gets window
  renderer <- gets renderer
  present renderer
  updateWindowSurface window
  state <- get
  t <- ticks
  put state { timer = t }

handleMenuInput :: KeyboardEventData -> Game ()
handleMenuInput e =
  case keysymKeycode (keyboardEventKeysym e) of
    KeycodeQ -> pushState exit
    KeycodeS -> pushState game
    _ -> return ()

handleGameInput :: KeyboardEventData -> Game ()
handleGameInput e =
  case keysymKeycode (keyboardEventKeysym e) of
    KeycodeEscape -> popState
    _ -> return ()
            
handleExitInput :: KeyboardEventData -> Game ()
handleExitInput e =
  case keysymKeycode (keyboardEventKeysym e) of
    KeycodeY -> clearStateStack
    KeycodeN -> popState
    _ -> return ()

handleInput :: (KeyboardEventData -> Game ()) -> Game ()
handleInput f = do
  event <- pollEvent
  case event of 
    Nothing -> return ()
    Just event ->
      case eventPayload event of
        QuitEvent -> clearStateStack
        KeyboardEvent k -> f k
        _ -> return ()

clearStateStack = do
  state <- get
  put state { statestack = [] }
  
pushState s = do
  state <- get
  stack <- gets statestack
  put state { statestack = s : stack }

popState = do
  state <- get
  stack <- gets statestack
  put state { statestack = tail stack }
