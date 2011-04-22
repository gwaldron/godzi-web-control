function Orientation(heading, pitch, roll)
{
  this.heading = heading;
  this.pitch = pitch;
  this.roll = roll;
}

Orientation.prototype.clone = function()
{
  return new Orientation(this.heading, this.pitch, this.roll);
}

Orientation.prototype.getHeading = function()
{
  return this.heading;
}

Orientation.prototype.setHeading = function(heading)
{
  this.heading = heading;
}

Orientation.prototype.getPitch = function()
{
  return this.pitch;
}

Orientation.prototype.setPitch = function(pitch)
{
  this.pitch = pitch;
}

Orientation.prototype.getRoll = function()
{
  return this.roll;
}

Orientation.prototype.setRoll = function(roll)
{
  this.roll = roll;
}