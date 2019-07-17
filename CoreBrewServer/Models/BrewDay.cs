using System.Collections.Generic;

namespace CoreBrewServer.Models
{
    public class BrewDay
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string StartTime { get; set; }
        public string ReachedMashTemp {get; set;}
        public string CompletedMash {get; set;}
        public string ReachedBoil {get; set;}
        public string ComnpletedBoil {get; set;}
        public int  BrewScheduleId {get; set;}
        public virtual BrewSchedule BrewSchedule { get; set; }

        public virtual List<MashTemperature> MashTemperatures { get; set; } = new List<MashTemperature>();
    }
}