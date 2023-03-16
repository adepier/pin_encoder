#include "pin_encoder.h" 

pin_encoder::pin_encoder(PinName pin , int num_compteur, encoder_values &encoder) : _interrupt(pin,PullDown)          // create the InterruptIn on the pin specified to MyInterrupt
    {   
         if (num_compteur==1)
        {
           // attach increment function of this counter instance
            _interrupt.rise(callback(this, &pin_encoder::increment)); 
            _interrupt.fall(callback(this, &pin_encoder::fall_1)); 
             }
        if (num_compteur==2)
        {
           // attach increment function of this counter instance
            _interrupt.rise(callback(this, &pin_encoder::rise_2)); 
            _interrupt.fall(callback(this, &pin_encoder::fall_2)); 
             }
        
        _encoder   = &encoder; 
        timer.start();
          previous_tick_ms = std::chrono::duration_cast<std::chrono::microseconds>(timer.elapsed_time()).count();
         _encoder->accel = 0;
         _encoder->speed = 0;
    }
/*principe:
il y a 2 compteurs 1 et 2
on dit compteur 1 rise = 0 / fall = 10
on dit compteur 2 rise = 21 / fall = 20

seul le compteur 1 rise augmente le compteur
dans le sens aller, on doit avoir la trame :  0 -> 21 -> 10 -> 20 =>  51
dans le sens retour, on doit avoir la trame : 0 -> 20 -> 10 -> 21 =>  51
*/

   void pin_encoder::fall_1()
{
      if (_encoder->tic_forward==21) {_encoder->tic_forward=31; };
      if (_encoder->tic_backward==20) {_encoder->tic_backward=30; };
}
void pin_encoder::fall_2()
{
   if (_encoder->tic_forward==31) {_encoder->tic_forward=51; };
   if (_encoder->tic_backward==0) {_encoder->tic_backward=20; };
     
}

void pin_encoder::rise_2()
{
   if (_encoder->tic_forward==0) {_encoder->tic_forward=21 ;};
   if (_encoder->tic_backward==30) {_encoder->tic_backward=51; };
     
}

void pin_encoder::increment()
{
      //on est arrivé a bout de la trame, on incremente le compteur
      if (_encoder->tic_forward==51){_encoder->count=_encoder->count+1;};
      if (_encoder->tic_backward==51){_encoder->count=_encoder->count-1;};
      
      //si le compteur a été incrémenté, on remet tout a zéro
      if (_encoder->tic_forward==51 || _encoder->tic_backward==51)
      {
          //calcul la vitesse et l'accélération
         int now = std::chrono::duration_cast<std::chrono::microseconds>(timer.elapsed_time()).count();
         float new_speed =( 1.0f / (now - previous_tick_ms) ) * 100000.0f;
         
         _encoder->accel = 10000.0f *(new_speed - _encoder->speed)/(now - previous_tick_ms);
         _encoder->speed = new_speed;
           previous_tick_ms = now;
          
          //RAZ des tic
          _encoder->tic_forward = 0;
          _encoder->tic_backward = 0;  
      }
}
 
 
