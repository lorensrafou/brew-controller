namespace CoreBrewServer.Models
{
    public class MashTemperature 
    {
        public int Id {get; set;}
        public string Timestamp {get; set;}
        public float Temperature {get; set;}
        public int BrewDayId {get; set;}
        public virtual BrewDay BrewDay {get; set;}
    }
}