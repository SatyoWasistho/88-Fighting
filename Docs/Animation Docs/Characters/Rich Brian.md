# Character Reference: Rich Brian
So Brian is like our mascot character, kind of the game's Jin Kazama. Average size, average movement, decently comfortable at every range. His design is really basic. Two things about animating him that are gonna be the most complicated are:

 - His coat. For any kind of fast, full-body motion, it basically follows cape physics.
 - His glasses. It can be tricky to draw them from certain angles at this low of a resolution. With this style, you're gonna find that the resolution makes some things easier and some things harder.

## Idle Animation
![Rich Brian Idle Pose](https://github.com/SatyoWasistho/88-Fighting/blob/master/Assets/Sprites/Characters/Rich%20Brian/Idle.png?raw=true)

So the first thing you're gonna do is make an idle animation from this pose. These are really simple to do, but it'll also give you a good feel for what's possible at this sizing. If you want some resources for this, AdamCYounis is a pixel art game animator who has some good videos about idles. These two do a really good job of showing what I'm looking for:

 - [AdamCYounis Character Idle Lecture](https://www.youtube.com/watch?v=vs-ierVdE7I&t=543s)

 - [AdamCYounis Sub-Pixel Animation Lecture](https://www.youtube.com/watch?v=cs1Gp61yafQ)

You don't have to watch these, but they're there if you need them.

## Walk Cycle

![Rich Brian Walk Pose](https://github.com/SatyoWasistho/88-Fighting/blob/master/Assets/Sprites/Characters/Rich%20Brian/Walk.png?raw=true)

For the walk cycle, I wanna mimick the walk from this music video at 0:25.

[Rich Brian - Love In My Pocket MV (Unfinished)](https://www.youtube.com/watch?v=qBG-tyMWNeo)

~20 frames sounds right, but feel free to adjust that number as you see fit. There's gonna be other animations later on where the frame-count is really strict, but this isn't one of them. 

I really wanna emphasize that back-and-forth sway in the upper body, the head-bob especially (this is where you're gonna get a lot of funny glasses angles). That's like where all the personality in the motion comes from.

## Crouch

![Rich Brian Crouch Pose](https://github.com/SatyoWasistho/88-Fighting/blob/master/Assets/Sprites/Characters/Rich%20Brian/Crouch.png?raw=true)

I actually want two animations for this.

 - **Transition Animation:** Just a 3-frame transition between his idle pose and his crouch pose. In Tekken terms, this is for his *while-standing* and *while-crouching* states. You can either do a unique animation for each, or you can do one animation that will just be played forward for one and backward for the other. The engine I've written supports either.

 - **Crouch Animation:** Same concept as the idle, just applied to this pose.

## TODO: Other Animations

I'll give you directions and resources for other animations later on. This is my first day off from the bar in a while, so I finally get to actually sit down and work on the game a bunch today.
