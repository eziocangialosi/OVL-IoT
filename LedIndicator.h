/**
 * @file        LedIndicator.h
 * @brief       Definition of LedIndicator class
 * @details     This class is used to manage the firebeetle debug led
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */

#ifndef LEDINDICATOR_H
#define LEDINDICATOR_H

#include <FastLED.h>

#define NUM_LEDS 1 //!< Number of leds used (needed by FastLed)
#define PIN 5 //!< Pin for led managing (needed by FastLed)

/**
 * @class       LedIndicator LedIndicator.h "LedIndicator.h"
 * @brief       Class that managing the debug led
 * @details     This led is used to send debug codes via the RGB led
 * @author      Ezio CANGIALOSI <eziocangialosi@gmail.com>
 * @version     dev
 * @date        04/2023
 */
class LedIndicator
  {
    public:
      /** @brief Constructor of LedIndicator class
       *  @details The constructor intialize the FastLED library and set the led in orange */
      LedIndicator();
      
      /** @brief Method to set the led to black
       *  @details This method just call setTo() method this black in param*/
      void setToBlack();
      
      /** @brief This method commands a SINGLE flash of the led in the desired color
       *  @param aColor the desired color
       *  @note If you are looking to make the led blink infinitely use blinkInfty() method */
      void blink(const CRGB aColor);
      
      /** @brief This method commands infinite flashes of the led in the desired color
       *  @param aColor the desired color
       *  @note If you are looking to make the led blink once use blink() method */
      void blinkInfty(const CRGB aColor);
      
      /** @brief Method to stop the led blinking */
      void stopBlinking();
      
      /** @brief Led managing loop
       *  @details This loop is needed when you want a led blinking, otherwise, it cannot*/
      void ledLoop();
      
      /** @brief Method to set the led in a color
       *  @param aColor the desired color */
      void setTo(const CRGB aColor);
      
      /** @brief Getter to know if the led is blinking
       *  @return True if it bliking false otherwise */
      bool isBlinking();
      
      /** @brief Method to call when a fatal error occured
       *  @details This method is an infinite loop, it used to hold the program
       *  @note For example, this loop is called when the sim card cannot be unlocked */
      void killLoop();
      
    private:
      CRGB led; //<! CRBG Object, used to store the current led color
      bool waitForBlack = false; //!< When the bliking mode is enabled, used to know if the next time should be a dark one
      bool blinkNonStop = false; //!< If we should blink infinitly
      CRGB blinkColor = CRGB::Black; //!< The color in witch we must blink
  };

#endif
